#include "MuDPGAnalysis/MuonDPGNtuples/src/MuNtupleGEMVFATStatusFiller.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/ESHandle.h"

MuNtupleGEMVFATStatusFiller::MuNtupleGEMVFATStatusFiller(edm::ConsumesCollector && collector,
					   const std::shared_ptr<MuNtupleConfig> config,
					   std::shared_ptr<TTree> tree, const std::string & label) :

  MuNtupleBaseFiller(config, tree, label)
{

  edm::InputTag & iTag = m_config->m_inputTags["gemOHStatus"];
  if (iTag.label() != "none") m_gemOHStatusToken = collector.consumes<MuonDigiCollection<GEMDetId,GEMOHStatus>>(iTag);

 

}

MuNtupleGEMVFATStatusFiller::~MuNtupleGEMVFATStatusFiller()
{

};

void MuNtupleGEMVFATStatusFiller::initialize()
{

  m_tree->Branch((m_label + "_nDigis").c_str(), &m_nDigis, (m_label + "_nDigis/i").c_str());

  m_tree->Branch((m_label + "_station").c_str(), &m_digi_station);
  m_tree->Branch((m_label + "_region").c_str(), &m_digi_region);
  m_tree->Branch((m_label + "_roll").c_str(), &m_digi_roll);
  m_tree->Branch((m_label + "_bx").c_str(), &m_digi_bx);
  m_tree->Branch((m_label + "_strip").c_str(), &m_digi_strip);
  m_tree->Branch((m_label + "_pad").c_str(), &m_digi_pad);

  m_tree->Branch((m_label + "_g_r").c_str(), &m_digi_g_r);
  m_tree->Branch((m_label + "_g_phi").c_str(), &m_digi_g_phi);
  m_tree->Branch((m_label + "g_eta").c_str(), &m_digi_g_eta);
  m_tree->Branch((m_label + "_g_x").c_str(), &m_digi_g_x);
  m_tree->Branch((m_label + "_g_y").c_str(), &m_digi_g_y);
  m_tree->Branch((m_label + "_g_z").c_str(), &m_digi_g_z);
  

}

void MuNtupleGEMVFATStatusFiller::clear()
{
  
  m_nDigis = 0;

  m_digi_station.clear();
  m_digi_roll.clear();
  m_digi_strip.clear();
  m_digi_bx.clear();
  m_digi_region.clear();
  m_digi_pad.clear();

  m_digi_g_r.clear();
  m_digi_g_phi.clear();
  m_digi_g_eta.clear();
  m_digi_g_x.clear();
  m_digi_g_y.clear();
  m_digi_g_z.clear();

}

void MuNtupleGEMVFATStatusFiller::fill(const edm::Event & ev)
{

  clear();

  const auto gem = m_config->m_gemGeometry;
  auto OH_StatusCollection = conditionalGet<MuonDigiCollection<GEMDetId,GEMOHStatus>>(ev, m_gemOHStatusToken,"OHStatus");

  if (OH_StatusCollection.isValid())
    {
      for (auto ohIt = OH_StatusCollection->begin(); ohIt != OH_StatusCollection->end(); ohIt++) \
	{
	  const GEMDetId& gem_id = (*ohIt).first;
	  const GEMOHStatusCollection::Range& range = (*ohIt).second;


      std::cout<<"Working on ohstatus region: " <<gem_id.region()<<"\tChamber: "<<gem_id.chamber()<<std::endl;

	  const GEMEtaPartition* roll = gem->etaPartition(gem_id);
	  const BoundPlane& surface = roll->surface();

	  for (auto digi = range.first; digi != range.second; ++digi) {

	    m_digi_station.push_back(gem_id.station());
	    m_digi_roll.push_back(gem_id.roll());
	    // m_digi_strip.push_back(digi->strip());
	    // m_digi_bx.push_back(digi->bx());

	    // m_digi_region.push_back(gem_id.region());

	    // const LocalPoint& local_pos = roll->centreOfStrip(digi->strip());
	    // const GlobalPoint& global_pos = surface.toGlobal(local_pos);

	    // m_digi_g_r.push_back(global_pos.perp());
	    // m_digi_g_phi.push_back(global_pos.phi());
	    // m_digi_g_eta.push_back(global_pos.eta());
	    // m_digi_g_x.push_back(global_pos.x());
	    // m_digi_g_y.push_back(global_pos.y());
	    // m_digi_g_z.push_back(global_pos.z());
	    
	    m_nDigis++;
	   

	  }

	}
      
    }

  
  return;

}
