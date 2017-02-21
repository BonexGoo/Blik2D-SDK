#pragma once
#include <blik_mutex.hpp>

namespace BLIK
{
    typedef void (*QueueFreeCB)(void*);

    //! \brief 큐지원
    template<typename TYPE, bool FORTHREAD = true, QueueFreeCB FREECB = nullptr, sint32 MAXCOUNT = 65536>
    class Queue
    {
    public:
        /*!
        \brief 큐에 1개 데이터적재
        \param data : 데이터
        */
        void Enqueue(TYPE data)
        {
            if(FORTHREAD) Mutex::Lock(DataMutex);
            {
                Head.Enqueue(data);
                DataCount++;
                BLIK_ASSERT("큐의 데이터수량이 경고수준입니다", DataCount != MAXCOUNT);
            }
            if(FORTHREAD) Mutex::Unlock(DataMutex);
        }

        /*!
        \brief 큐에서 1개 데이터배출
        \param nullvalue : 데이터가 없을 경우의 값
        \return 해당 데이터
        */
        TYPE Dequeue(TYPE nullvalue = TYPE())
        {
            TYPE Data = nullvalue;
            if(FORTHREAD) Mutex::Lock(DataMutex);
            if(0 < DataCount)
            {
                Data = Head.Dequeue();
                DataCount--;
            }
            if(FORTHREAD) Mutex::Unlock(DataMutex);
            return Data;
        }

        /*!
        \brief 현재 큐에 적재된 데이터수량
        \return 데이터수량
        */
        sint32 Count()
        {
            sint32 Result = 0;
            if(FORTHREAD) Mutex::Lock(DataMutex);
            {
                Result = DataCount;
            }
            if(FORTHREAD) Mutex::Unlock(DataMutex);
            return Result;
        }

        /*!
        \brief 생성자
        */
        Queue() : Head(TYPE())
        {
            DataCount = 0;
            if(FORTHREAD) DataMutex = Mutex::Open();
        }

        /*!
        \brief 소멸자
        */
        ~Queue()
        {
            if(FORTHREAD) Mutex::Close(DataMutex);
            if(FREECB)
            {
                TYPE OldData = Dequeue();
                while(!!OldData)
                {
                    FREECB(AnyTypeToPtr(OldData));
                    OldData = Dequeue();
                }
            }
            else BLIK_ASSERT("큐에 해제되지 않은 데이터가 존재합니다", DataCount == 0);
        }

    private:
        class QueueElement
        {
        public:
            TYPE Data;
            QueueElement* Prev;
            QueueElement* Next;

        public:
            QueueElement(TYPE data) : Data(data), Prev(this), Next(this) {}

            ~QueueElement()
            {
                if(Prev) Prev->Next = nullptr;
                QueueElement* CurrNode = nullptr;
                QueueElement* NextNode = Next;
                while((CurrNode = NextNode) != nullptr)
                {
                    NextNode = CurrNode->Next;
                    CurrNode->Prev = nullptr;
                    CurrNode->Next = nullptr;
                    delete CurrNode;
                }
            }

        public:
            void Enqueue(TYPE data)
            {
                QueueElement* NewNode = new QueueElement(data);
                (NewNode->Prev = Prev)->Next = NewNode;
                (NewNode->Next = this)->Prev = NewNode;
            }

            TYPE Dequeue()
            {
                QueueElement* DeleteNode = Next;
                DeleteNode->Next->Prev = DeleteNode->Prev;
                DeleteNode->Prev->Next = DeleteNode->Next;
                DeleteNode->Prev = nullptr;
                DeleteNode->Next = nullptr;
                TYPE Result = DeleteNode->Data;
                delete DeleteNode;
                return Result;
            }
        };

    private:
        QueueElement Head;
        sint32 DataCount;
        id_mutex DataMutex;
    };
}
