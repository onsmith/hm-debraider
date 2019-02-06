#include "TDbrCabacContexts.h"


TDbrCabacContexts::TDbrCabacContexts() :
  numContextModels                         (0),
  m_cCUSplitFlagSCModel                      (1, 1,                      NUM_SPLIT_FLAG_CTX,                 contextModels + numContextModels, numContextModels),
  m_cCUSkipFlagSCModel                       (1, 1,                      NUM_SKIP_FLAG_CTX,                  contextModels + numContextModels, numContextModels),
  m_cCUMergeFlagExtSCModel                   (1, 1,                      NUM_MERGE_FLAG_EXT_CTX,             contextModels + numContextModels, numContextModels),
  m_cCUMergeIdxExtSCModel                    (1, 1,                      NUM_MERGE_IDX_EXT_CTX,              contextModels + numContextModels, numContextModels),
  m_cCUPartSizeSCModel                       (1, 1,                      NUM_PART_SIZE_CTX,                  contextModels + numContextModels, numContextModels),
  m_cCUPredModeSCModel                       (1, 1,                      NUM_PRED_MODE_CTX,                  contextModels + numContextModels, numContextModels),
  m_cCUIntraPredSCModel                      (1, 1,                      NUM_INTRA_PREDICT_CTX,              contextModels + numContextModels, numContextModels),
  m_cCUChromaPredSCModel                     (1, 1,                      NUM_CHROMA_PRED_CTX,                contextModels + numContextModels, numContextModels),
  m_cCUDeltaQpSCModel                        (1, 1,                      NUM_DELTA_QP_CTX,                   contextModels + numContextModels, numContextModels),
  m_cCUInterDirSCModel                       (1, 1,                      NUM_INTER_DIR_CTX,                  contextModels + numContextModels, numContextModels),
  m_cCURefPicSCModel                         (1, 1,                      NUM_REF_NO_CTX,                     contextModels + numContextModels, numContextModels),
  m_cCUMvdSCModel                            (1, 1,                      NUM_MV_RES_CTX,                     contextModels + numContextModels, numContextModels),
  m_cCUQtCbfSCModel                          (1, NUM_QT_CBF_CTX_SETS,    NUM_QT_CBF_CTX_PER_SET,             contextModels + numContextModels, numContextModels),
  m_cCUTransSubdivFlagSCModel                (1, 1,                      NUM_TRANS_SUBDIV_FLAG_CTX,          contextModels + numContextModels, numContextModels),
  m_cCUQtRootCbfSCModel                      (1, 1,                      NUM_QT_ROOT_CBF_CTX,                contextModels + numContextModels, numContextModels),
  m_cCUSigCoeffGroupSCModel                  (1, 2,                      NUM_SIG_CG_FLAG_CTX,                contextModels + numContextModels, numContextModels),
  m_cCUSigSCModel                            (1, 1,                      NUM_SIG_FLAG_CTX,                   contextModels + numContextModels, numContextModels),
  m_cCuCtxLastX                              (1, NUM_CTX_LAST_FLAG_SETS, NUM_CTX_LAST_FLAG_XY,               contextModels + numContextModels, numContextModels),
  m_cCuCtxLastY                              (1, NUM_CTX_LAST_FLAG_SETS, NUM_CTX_LAST_FLAG_XY,               contextModels + numContextModels, numContextModels),
  m_cCUOneSCModel                            (1, 1,                      NUM_ONE_FLAG_CTX,                   contextModels + numContextModels, numContextModels),
  m_cCUAbsSCModel                            (1, 1,                      NUM_ABS_FLAG_CTX,                   contextModels + numContextModels, numContextModels),
  m_cMVPIdxSCModel                           (1, 1,                      NUM_MVP_IDX_CTX,                    contextModels + numContextModels, numContextModels),
  m_cSaoMergeSCModel                         (1, 1,                      NUM_SAO_MERGE_FLAG_CTX,             contextModels + numContextModels, numContextModels),
  m_cSaoTypeIdxSCModel                       (1, 1,                      NUM_SAO_TYPE_IDX_CTX,               contextModels + numContextModels, numContextModels),
  m_cTransformSkipSCModel                    (1, MAX_NUM_CHANNEL_TYPE,   NUM_TRANSFORMSKIP_FLAG_CTX,         contextModels + numContextModels, numContextModels),
  m_CUTransquantBypassFlagSCModel            (1, 1,                      NUM_CU_TRANSQUANT_BYPASS_FLAG_CTX,  contextModels + numContextModels, numContextModels),
  m_explicitRdpcmFlagSCModel                 (1, MAX_NUM_CHANNEL_TYPE,   NUM_EXPLICIT_RDPCM_FLAG_CTX,        contextModels + numContextModels, numContextModels),
  m_explicitRdpcmDirSCModel                  (1, MAX_NUM_CHANNEL_TYPE,   NUM_EXPLICIT_RDPCM_DIR_CTX,         contextModels + numContextModels, numContextModels),
  m_cCrossComponentPredictionSCModel         (1, 1,                      NUM_CROSS_COMPONENT_PREDICTION_CTX, contextModels + numContextModels, numContextModels),
  m_ChromaQpAdjFlagSCModel                   (1, 1,                      NUM_CHROMA_QP_ADJ_FLAG_CTX,         contextModels + numContextModels, numContextModels),
  m_ChromaQpAdjIdcSCModel                    (1, 1,                      NUM_CHROMA_QP_ADJ_IDC_CTX,          contextModels + numContextModels, numContextModels) {
  assert(numContextModels <= MAX_NUM_CTX_MOD);
}




ContextModel3DBuffer& TDbrCabacContexts::get(TDbrCabacContexts::SyntaxElement element) {
  switch (element) {
  case SyntaxElement::CuSplitFlag:
    return m_cCUSplitFlagSCModel;
  case SyntaxElement::CuSkipFlag:
    return m_cCUSkipFlagSCModel;
  case SyntaxElement::CuMergeFlag:
    return m_cCUMergeFlagExtSCModel;
  case SyntaxElement::CuMergeIndex:
    return m_cCUMergeIdxExtSCModel;
  case SyntaxElement::CuPartSize:
    return m_cCUPartSizeSCModel;
  case SyntaxElement::CuPredMode:
    return m_cCUPredModeSCModel;
  case SyntaxElement::CuIntraPred:
    return m_cCUIntraPredSCModel;
  case SyntaxElement::CuChromaPred:
    return m_cCUChromaPredSCModel;
  case SyntaxElement::CuDeltaQp:
    return m_cCUDeltaQpSCModel;
  case SyntaxElement::CuInterDir:
    return m_cCUInterDirSCModel;
  case SyntaxElement::CuRefPic:
    return m_cCURefPicSCModel;
  case SyntaxElement::CuMvd:
    return m_cCUMvdSCModel;
  case SyntaxElement::QtCbf:
    return m_cCUQtCbfSCModel;
  case SyntaxElement::TuSubdivFlag:
    return m_cCUTransSubdivFlagSCModel;
  case SyntaxElement::QtRootCbf:
    return m_cCUQtRootCbfSCModel;

  case SyntaxElement::SigCoeffGroupFlag:
    return m_cCUSigCoeffGroupSCModel;
  case SyntaxElement::SigCoeffFlag:
    return m_cCUSigSCModel;
  case SyntaxElement::LastSigCoeffX:
    return m_cCuCtxLastX;
  case SyntaxElement::LastSigCoeffY:
    return m_cCuCtxLastY;
  case SyntaxElement::CoeffGt1Flag:
    return m_cCUOneSCModel;
  case SyntaxElement::CoeffGt2Flag:
    return m_cCUAbsSCModel;

  case SyntaxElement::MvpIndex:
    return m_cMVPIdxSCModel;

  case SyntaxElement::SaoMerge:
    return m_cSaoMergeSCModel;
  case SyntaxElement::SaoTypeIndex:
    return m_cSaoTypeIdxSCModel;
  case SyntaxElement::TransformSkip:
    return m_cTransformSkipSCModel;
  case SyntaxElement::CuTransquantBypassFlag:
    return m_CUTransquantBypassFlagSCModel;
  case SyntaxElement::ExplicitRdpcmFlag:
    return m_explicitRdpcmFlagSCModel;
  case SyntaxElement::ExplicitRdpcmDir:
    return m_explicitRdpcmDirSCModel;
  case SyntaxElement::CrossComponentPrediction:
    return m_cCrossComponentPredictionSCModel;

  case SyntaxElement::ChromaQpAdjFlag:
    return m_ChromaQpAdjFlagSCModel;
  case SyntaxElement::ChromaQpAdjIdc:
    return m_ChromaQpAdjIdcSCModel;
  }
}


UInt* TDbrCabacContexts::getGolombRiceStats() {
  return m_golombRiceAdaptationStatistics;
}



UInt& TDbrCabacContexts::getGolombRiceStats(Int i) {
  return m_golombRiceAdaptationStatistics[i];
}



Void TDbrCabacContexts::resetEntropy(TComSlice* pSlice) {
  SliceType sliceType = pSlice->getSliceType();
  Int       qp        = pSlice->getSliceQp();

  if (pSlice->getPPS()->getCabacInitPresentFlag() && pSlice->getCabacInitFlag()) {
    switch (sliceType) {
    // Change initialization table to B_SLICE initialization
    case P_SLICE:
      sliceType = B_SLICE;
      break;

    // Change initialization table to P_SLICE initialization
    case B_SLICE:           
      sliceType = P_SLICE;
      break;

    // Should never happen
    default:
      assert(0);
      break;
    }
  }

  m_cCUSplitFlagSCModel.initBuffer             (sliceType, qp, (UChar*) INIT_SPLIT_FLAG);
  m_cCUSkipFlagSCModel.initBuffer              (sliceType, qp, (UChar*) INIT_SKIP_FLAG);
  m_cCUMergeFlagExtSCModel.initBuffer          (sliceType, qp, (UChar*) INIT_MERGE_FLAG_EXT);
  m_cCUMergeIdxExtSCModel.initBuffer           (sliceType, qp, (UChar*) INIT_MERGE_IDX_EXT);
  m_cCUPartSizeSCModel.initBuffer              (sliceType, qp, (UChar*) INIT_PART_SIZE);
  m_cCUPredModeSCModel.initBuffer              (sliceType, qp, (UChar*) INIT_PRED_MODE);
  m_cCUIntraPredSCModel.initBuffer             (sliceType, qp, (UChar*) INIT_INTRA_PRED_MODE);
  m_cCUChromaPredSCModel.initBuffer            (sliceType, qp, (UChar*) INIT_CHROMA_PRED_MODE);
  m_cCUInterDirSCModel.initBuffer              (sliceType, qp, (UChar*) INIT_INTER_DIR);
  m_cCUMvdSCModel.initBuffer                   (sliceType, qp, (UChar*) INIT_MVD);
  m_cCURefPicSCModel.initBuffer                (sliceType, qp, (UChar*) INIT_REF_PIC);
  m_cCUDeltaQpSCModel.initBuffer               (sliceType, qp, (UChar*) INIT_DQP);
  m_cCUQtCbfSCModel.initBuffer                 (sliceType, qp, (UChar*) INIT_QT_CBF);
  m_cCUQtRootCbfSCModel.initBuffer             (sliceType, qp, (UChar*) INIT_QT_ROOT_CBF);
  m_cCUSigCoeffGroupSCModel.initBuffer         (sliceType, qp, (UChar*) INIT_SIG_CG_FLAG);
  m_cCUSigSCModel.initBuffer                   (sliceType, qp, (UChar*) INIT_SIG_FLAG);
  m_cCuCtxLastX.initBuffer                     (sliceType, qp, (UChar*) INIT_LAST);
  m_cCuCtxLastY.initBuffer                     (sliceType, qp, (UChar*) INIT_LAST);
  m_cCUOneSCModel.initBuffer                   (sliceType, qp, (UChar*) INIT_ONE_FLAG);
  m_cCUAbsSCModel.initBuffer                   (sliceType, qp, (UChar*) INIT_ABS_FLAG);
  m_cMVPIdxSCModel.initBuffer                  (sliceType, qp, (UChar*) INIT_MVP_IDX);
  m_cSaoMergeSCModel.initBuffer                (sliceType, qp, (UChar*) INIT_SAO_MERGE_FLAG);
  m_cSaoTypeIdxSCModel.initBuffer              (sliceType, qp, (UChar*) INIT_SAO_TYPE_IDX);
  m_cCUTransSubdivFlagSCModel.initBuffer       (sliceType, qp, (UChar*) INIT_TRANS_SUBDIV_FLAG);
  m_cTransformSkipSCModel.initBuffer           (sliceType, qp, (UChar*) INIT_TRANSFORMSKIP_FLAG);
  m_CUTransquantBypassFlagSCModel.initBuffer   (sliceType, qp, (UChar*) INIT_CU_TRANSQUANT_BYPASS_FLAG);
  m_explicitRdpcmFlagSCModel.initBuffer        (sliceType, qp, (UChar*) INIT_EXPLICIT_RDPCM_FLAG);
  m_explicitRdpcmDirSCModel.initBuffer         (sliceType, qp, (UChar*) INIT_EXPLICIT_RDPCM_DIR);
  m_cCrossComponentPredictionSCModel.initBuffer(sliceType, qp, (UChar*) INIT_CROSS_COMPONENT_PREDICTION);
  m_ChromaQpAdjFlagSCModel.initBuffer          (sliceType, qp, (UChar*) INIT_CHROMA_QP_ADJ_FLAG);
  m_ChromaQpAdjIdcSCModel.initBuffer           (sliceType, qp, (UChar*) INIT_CHROMA_QP_ADJ_IDC);

  for (UInt statisticIndex = 0; statisticIndex < RExt__GOLOMB_RICE_ADAPTATION_STATISTICS_SETS; statisticIndex++) {
    m_golombRiceAdaptationStatistics[statisticIndex] = 0;
  }

  // m_pcTDecBinIf->start();
}
