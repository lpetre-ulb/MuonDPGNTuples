#ifndef MuNtuple_MuNtupleGEMVFATStatusFiller_h
#define MuNtuple_MuNtupleGEMVFATStatusFiller_h


#include "MuDPGAnalysis/MuonDPGNtuples/src/MuNtupleBaseFiller.h"

#include "Geometry/Records/interface/MuonGeometryRecord.h"
#include "Geometry/GEMGeometry/interface/GEMGeometry.h"

#include "DataFormats/GEMDigi/interface/GEMDigiCollection.h"
#include "DataFormats/GEMDigi/interface/GEMOHStatusCollection.h"

#include "FWCore/Framework/interface/ConsumesCollector.h"

#include <vector>

class MuNtupleGEMVFATStatusFiller : public MuNtupleBaseFiller
{
 public: 

  //Constructor
  MuNtupleGEMVFATStatusFiller(edm::ConsumesCollector && collector,
                      const std::shared_ptr<MuNtupleConfig> config,
                      std::shared_ptr<TTree> tree, const std::string & label);

  
  //Destructor
  virtual ~MuNtupleGEMVFATStatusFiller();

  /// Intialize function : setup tree branches etc ...
  virtual void initialize() final;

  /// Clear branches before event filling
  virtual void clear() final;

  /// Fill tree branches for a given events
  virtual void fill(const edm::Event & ev) final;
  

 private:
  
  
  /// The digi token
  edm::EDGetTokenT<MuonDigiCollection<GEMDetId,GEMOHStatus>> m_gemOHStatusToken;
  edm::ESGetToken<GEMGeometry, MuonGeometryRecord> geomToken_;
  /// The variables holding
  /// all digi related information

  unsigned int m_nDigis; // the # of digis (size of all following vectors)

  std::vector<short>  m_digi_station;
  std::vector<short>  m_digi_roll;
  std::vector<short>  m_digi_strip;
  std::vector<short>  m_digi_bx;
  std::vector<short>  m_digi_region;
  std::vector<short>  m_digi_pad;

  std::vector<float> m_digi_g_r;
  std::vector<float> m_digi_g_phi;
  std::vector<float> m_digi_g_eta;
  std::vector<float> m_digi_g_x;
  std::vector<float> m_digi_g_y;
  std::vector<float> m_digi_g_z;

};

#endif


  
