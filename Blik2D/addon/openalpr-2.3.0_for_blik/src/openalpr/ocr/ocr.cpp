/*
 * Copyright (c) 2015 OpenALPR Technology, Inc.
 * Open source Automated License Plate Recognition [http://www.openalpr.com]
 *
 * This file is part of OpenALPR.
 *
 * OpenALPR is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License
 * version 3 as published by the Free Software Foundation
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "ocr.h"

namespace alpr
{
  
  OCR::OCR(Config* config) : postProcessor(config) {
    this->config = config;
  }


  OCR::~OCR() {
  }

  
  void OCR::performOCR(PipelineData* pipeline_data)
  {
    timespec startTime;
    getTimeMonotonic(&startTime);

    segment(pipeline_data);
    
    postProcessor.clear();

    int absolute_charpos = 0;
	int charlength = 0; // BLIK: 라인개념 대신 라스트인덱스개념으로 변경
    for (unsigned int line_idx = 0; line_idx < pipeline_data->textLines.size(); line_idx++)
    {
      std::vector<OcrChar> chars = recognize_line(line_idx, pipeline_data);
      
      for (uint32_t i = 0; i < chars.size(); i++)
	  {
		postProcessor.addLetter(chars[i].letter, line_idx, charlength + chars[i].char_index, chars[i].confidence);
        // BLIK: 윗줄의 원본(postProcessor.addLetter(chars[i].letter, line_idx, chars[i].char_index, chars[i].confidence);)
	  }
	  charlength += pipeline_data->charRegions[line_idx].size(); // BLIK: 라인개념 대신 라스트인덱스개념으로 변경
    }
    
    if (config->debugTiming)
    {
      timespec endTime;
      getTimeMonotonic(&endTime);
      std::cout << "OCR Time: " << diffclock(startTime, endTime) << "ms." << std::endl;
    }
  }
}