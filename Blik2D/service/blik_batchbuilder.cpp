#include <blik.hpp>
#include "blik_batchbuilder.hpp"

#include <platform/blik_platform.hpp>

namespace BLIK
{
    BatchBuilder::BatchBuilder(chars dirname) : m_dirname(dirname)
    {
        m_ref_dsts = nullptr;
        m_ref_srcs = nullptr;
        m_ref_comment = nullptr;
    }

    BatchBuilder::~BatchBuilder()
    {
    }

    void BatchBuilder::Processing(chars filename)
    {
        String Command = ReadFile(filename);
        const String FindKey = "_BLIK_BATCH_COMMAND_";
        sint32 FindedPos = Command.Find(0, FindKey);
        if(0 <= FindedPos)
        {
            Property JsonCommand(ST_Json, SO_OnlyReference, ((chars) Command) + FindedPos + FindKey.Length());
            if(!String::Compare(JsonCommand("type").GetString(), "include-alias"))
                Process_IncludeAlias(Command, FindedPos, JsonCommand("prm").GetString(),
                    JsonCommand("restore-comment").GetString(nullptr));
        }
    }

    void BatchBuilder::Process_IncludeAlias(const String& command, sint32 pos, chars prm, chars comment)
    {
        // 기호수집
        String FindPrm = String("#define ") + prm;
        Strings AliasDsts;
        Strings AliasSrcs;
        while(-1 < (pos = command.Find(pos, FindPrm)))
        {
            chars FindFocus = &((chars) command)[pos] + FindPrm.Length();
            String& NewDst = AliasDsts.AtAdding();
            String& NewSrc = AliasSrcs.AtAdding();
            NewSrc += prm;
            // <>/""기호 수집
            if(FindFocus[0] == 'V' && FindFocus[1] == '_')
            {
                NewDst += '<';
                NewSrc += "V_";
                FindFocus += 2;
            }
            else if(FindFocus[0] == 'U' && FindFocus[1] == '_')
            {
                NewDst += '\"';
                NewSrc += "U_";
                FindFocus += 2;
            }
            // 패스기호 수집
            while(*FindFocus != ' ' && *FindFocus != '\t')
            {
                // 치환기호
                if(FindFocus[0] == '_')
                {
                    if(FindFocus[1] == '_')
                    {
                        NewDst += '/';
                        NewSrc += "__";
                        FindFocus += 2;
                        continue;
                    }
                    else if(FindFocus[1] == 'h' && (FindFocus[2] == ' ' || FindFocus[2] == '\t'))
                    {
                        NewDst += ".h";
                        NewSrc += "_h";
                        FindFocus += 2;
                        continue;
                    }
					else if(FindFocus[1] == 'h' && FindFocus[2] == 'p' && FindFocus[3] == 'p' && (FindFocus[4] == ' ' || FindFocus[4] == '\t'))
                    {
                        NewDst += ".hpp";
                        NewSrc += "_hpp";
                        FindFocus += 4;
                        continue;
                    }
					else if(FindFocus[1] == 'i' && FindFocus[2] == 'n' && FindFocus[3] == 'c' && (FindFocus[4] == ' ' || FindFocus[4] == '\t'))
                    {
                        NewDst += ".inc";
                        NewSrc += "_inc";
                        FindFocus += 4;
                        continue;
                    }
                    else if(FindFocus[1] == 'M' && FindFocus[2] == '_')
                    {
                        NewDst += "-";
                        NewSrc += "_M_";
                        FindFocus += 3;
                        continue;
                    }
					else if(FindFocus[1] == 'D' && FindFocus[2] == '_')
                    {
                        NewDst += ".";
                        NewSrc += "_D_";
                        FindFocus += 3;
                        continue;
                    }
                }
                // 소문자화
                if('A' <= *FindFocus && *FindFocus <= 'Z')
                    NewDst += *FindFocus - 'A' + 'a';
                else NewDst += *FindFocus;
                NewSrc += *FindFocus;
                FindFocus++;
            }
            pos += FindFocus - &((chars) command)[pos];
        }

        // 폴더순회
        m_ref_dsts = &AliasDsts;
        m_ref_srcs = &AliasSrcs;
        m_ref_comment = comment;
        Platform::File::Search(m_dirname, Process_IncludeAliasCB, this, true);
    }

    void BatchBuilder::Process_IncludeAliasCB(chars itemname, payload data)
    {
        if(Platform::File::ExistForDir(itemname))
            Platform::File::Search(itemname, Process_IncludeAliasCB, data, true);
        else
        {
            const String Filename = itemname;
            if(!String::CompareNoCase(".c", Filename.Right(2))
				|| !String::CompareNoCase(".cc", Filename.Right(3))
				|| !String::CompareNoCase(".cpp", Filename.Right(4))
				|| !String::CompareNoCase(".h", Filename.Right(2))
				|| !String::CompareNoCase(".hpp", Filename.Right(4)))
            {
                BatchBuilder* This = (BatchBuilder*) data;
                This->Build_IncludeAlias(itemname, *This->m_ref_dsts, *This->m_ref_srcs, This->m_ref_comment);
            }
        }
    }

    void BatchBuilder::Build_IncludeAlias(chars sourcepath, const Strings& dsts, const Strings& srcs, chars comment)
    {
        String SourceCode = ReadFile(sourcepath);
        String ResultCode;

        // 결과구성
        sint32 FindFocus = 0, CopyFocus = 0;
        String FindKey = "include";
        chars SourcePtr = (chars) SourceCode;
        while(-1 < (FindFocus = SourceCode.Find(FindFocus, FindKey)))
        {
            // #여부 확인
            bool HasSharp = false;
            for(sint32 i = FindFocus - 1; 0 <= i; --i)
            {
                if(SourcePtr[i] == '#')
                {
                    HasSharp = true;
                    break;
                }
                else if(SourcePtr[i] != ' ' && SourcePtr[i] != '\t')
                    break;
            }

            // 검색키/공백 스킵
            FindFocus += FindKey.Length();
            while(SourcePtr[FindFocus] == ' ' || SourcePtr[FindFocus] == '\t')
                FindFocus++;

            if(HasSharp)
            {
                // 매칭분석으로 비교대상 도출
                sint32 FindEnd = FindFocus + 1;
                if(SourcePtr[FindFocus] == '\"')
                    while(SourcePtr[FindEnd] && SourcePtr[FindEnd] != '\n' && SourcePtr[FindEnd] != '\"') FindEnd++;
                else if(SourcePtr[FindFocus] == '<')
                    while(SourcePtr[FindEnd] && SourcePtr[FindEnd] != '\n' && SourcePtr[FindEnd] != '>') FindEnd++;
                else continue;
                // 매칭실패
                if(!SourcePtr[FindEnd] || SourcePtr[FindEnd] == '\n')
                    continue;

                // 비교대상의 정규화
                String IncludePath = String(&SourcePtr[FindFocus], FindEnd - FindFocus);
                char* IncludePathFocus = (char*)(chars) IncludePath;
                while(*IncludePathFocus)
                {
                    // 특수문자(UTF-8/CP949)가 아닐 경우
                    if(~*IncludePathFocus & 0x80)
                    {
                        // 소문자화
                        if('A' <= *IncludePathFocus && *IncludePathFocus <= 'Z')
                            *IncludePathFocus += 'a' - 'A';
                        // 슬래시전환
                        else if(*IncludePathFocus == '\\')
                            *IncludePathFocus = '/';
                    }
                    IncludePathFocus++;
                }

                // 비교대상을 변경목록에서 찾기(역순탐색)
                for(sint32 i = dsts.Count() - 1; 0 <= i; --i)
                    if(!dsts[i].Compare(IncludePath))
                    {
                        // 지연시킨 복사수행
                        ResultCode.Add(&SourcePtr[CopyFocus], FindFocus - CopyFocus);
                        CopyFocus = FindEnd + 1;
                        // 변경목록 수정안을 복사
                        ResultCode += srcs[i];
                        // 원본(비교대상)을 주석화
                        if(comment)
                        {
                            ResultCode += comment;
                            ResultCode.Add(&SourcePtr[FindFocus], FindEnd - FindFocus + 1);
                        }
                        break;
                    }
            }
        }

        // 결과가 존재하면 파일로 구성
        if(0 < ResultCode.Length())
        {
            String NewSourcePath = m_dirname + "_batched" + &sourcepath[m_dirname.Length()];
            id_file NewSourceFileID = Platform::File::OpenForWrite(NewSourcePath, true);
            Platform::File::Write(NewSourceFileID, (bytes)(chars) ResultCode, ResultCode.Length());
            Platform::File::Write(NewSourceFileID, (bytes) &SourcePtr[CopyFocus], SourceCode.Length() - CopyFocus);
            Platform::File::Close(NewSourceFileID);
        }
    }

    String BatchBuilder::ReadFile(chars filename)
    {
        id_file_read OptionFileID = Platform::File::OpenForRead(filename);
        const sint32 OptionFileSize = Platform::File::Size(OptionFileID);
        chararray CommandWords;
        CommandWords.AtWherever(OptionFileSize) = '\0';
        Platform::File::Read(OptionFileID, (uint08*) CommandWords.AtDumping(0, OptionFileSize + 1), OptionFileSize);
        Platform::File::Close(OptionFileID);
        return String(CommandWords);
    }
}
