///////////////////////////////////////////////////////////////////////
// File:        lm_pain_points.h
// Description: Functions that utilize the knowledge about the properties
//              of the paths explored by the segmentation search in order
//              to generate "pain points" - the locations in the ratings
//              matrix which should be classified next.
// Author:      Rika Antonova
// Created:     Mon Jun 20 11:26:43 PST 2012
//
// (C) Copyright 2012, Google Inc.
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
///////////////////////////////////////////////////////////////////////

#ifndef TESSERACT_WORDREC_PAIN_POINTS_H_
#define TESSERACT_WORDREC_PAIN_POINTS_H_

#include "associate.h"
#include BLIK_TESSERACT_U_dict_h //original-code:"dict.h"
#include BLIK_TESSERACT_U_genericheap_h //original-code:"genericheap.h"
#include "lm_state.h"

namespace tesseract {

// Heap of pain points used for determining where to chop/join.
typedef GenericHeap<MatrixCoordPair> PainPointHeap;

// Types of pain points (ordered in the decreasing level of importance).
enum LMPainPointsType {
  LM_PPTYPE_BLAMER,
  LM_PPTYPE_AMBIG,
  LM_PPTYPE_PATH,
  LM_PPTYPE_SHAPE,

  LM_PPTYPE_NUM
};

static const char * const LMPainPointsTypeName[] = {
    "LM_PPTYPE_BLAMER",
    "LM_PPTYPE_AMBIGS",
    "LM_PPTYPE_PATH",
    "LM_PPTYPE_SHAPE",
};

class LMPainPoints {
 public:

  static const float kDefaultPainPointPriorityAdjustment;
  // If there is a significant drop in character ngram probability or a
  // dangerous ambiguity make the thresholds on what blob combinations
  // can be classified looser.
  static const float kLooseMaxCharWhRatio;
  // Returns a description of the type of a pain point.
  static const char* PainPointDescription(LMPainPointsType type) {
    return LMPainPointsTypeName[type];
  }

  LMPainPoints(int max, float rat, bool fp, const Dict *d, int deb) :
    max_heap_size_(max), max_char_wh_ratio_(rat), fixed_pitch_(fp),
    dict_(d), debug_level_(deb) {}
  ~LMPainPoints() {}

  // Returns true if the heap of pain points of pp_type is not empty().
  inline bool HasPainPoints(LMPainPointsType pp_type) const {
    return !pain_points_heaps_[pp_type].empty();
  }

  // Dequeues the next pain point from the pain points queue and copies
  // its contents and priority to *pp and *priority.
  // Returns LM_PPTYPE_NUM if pain points queue is empty, otherwise the type.
  LMPainPointsType Deque(MATRIX_COORD *pp, float *priority);

  // Clears pain points heap.
  void Clear() {
    for (int h = 0; h < LM_PPTYPE_NUM; ++h) pain_points_heaps_[h].clear();
  }

  // For each cell, generate a "pain point" if the cell is not classified
  // and has a left or right neighbor that was classified.
  void GenerateInitial(WERD_RES *word_res);

  // Generate pain points from the given path.
  void GenerateFromPath(float rating_cert_scale, ViterbiStateEntry *vse,
                        WERD_RES *word_res);

  // Generate pain points from dangerous ambiguities in best choice.
  void GenerateFromAmbigs(const DANGERR &fixpt, ViterbiStateEntry *vse,
                          WERD_RES *word_res);

  // Generate a pain point for the blamer.
  bool GenerateForBlamer(double max_char_wh_ratio, WERD_RES *word_res,
                         int col, int row) {
    return GeneratePainPoint(col, row, LM_PPTYPE_BLAMER, 0.0, false,
                             max_char_wh_ratio, word_res);
  }

  // Adds a pain point to classify chunks_record->ratings(col, row).
  // Returns true if a new pain point was added to an appropriate heap.
  // Pain point priority is set to special_priority for pain points of
  // LM_PPTYPE_AMBIG or LM_PPTYPE_PATH, for other pain points
  // AssociateStats::gap_sum is used.
  bool GeneratePainPoint(int col, int row, LMPainPointsType pp_type,
                         float special_priority, bool ok_to_extend,
                         float max_char_wh_ratio,
                         WERD_RES *word_res);

  // Adjusts the pain point coordinates to cope with expansion of the ratings
  // matrix due to a split of the blob with the given index.
  void RemapForSplit(int index);

 private:
  // Priority queues containing pain points generated by the language model
  // The priority is set by the language model components, adjustments like
  // seam cost and width priority are factored into the priority.
  PainPointHeap pain_points_heaps_[LM_PPTYPE_NUM];
  // Maximum number of points to keep in the heap.
  int max_heap_size_;
  // Maximum character width/height ratio.
  float max_char_wh_ratio_;
  // Set to true if fixed pitch should be assumed.
  bool fixed_pitch_;
  // Cached pointer to dictionary.
  const Dict *dict_;
  // Debug level for print statements.
  int debug_level_;
};

}  // namespace tesseract

#endif  // TESSERACT_WORDREC_PAIN_POINTS_H_