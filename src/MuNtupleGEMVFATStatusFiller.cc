#include "MuDPGAnalysis/MuonDPGNtuples/src/MuNtupleGEMVFATStatusFiller.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/ESHandle.h"

MuNtupleGEMVFATStatusFiller::MuNtupleGEMVFATStatusFiller(edm::ConsumesCollector &&collector,
                                                         const std::shared_ptr<MuNtupleConfig> config,
                                                         std::shared_ptr<TTree> tree, const std::string &label) : MuNtupleBaseFiller(config, tree, label)
{
    // I couldn't manage to get the GEM OH Status collections with the usual conditionalGet. So I have copied DQM https://github.com/cms-sw/cmssw/blob/38405a5b319be8ec094c981d6b45320aa577676a/DQM/GEM/plugins/GEMDAQStatusSource.cc#L9
    m_gemOHStatusToken = collector.consumes<GEMOHStatusCollection>(edm::InputTag("muonGEMDigis", "OHStatus"));
}

MuNtupleGEMVFATStatusFiller::~MuNtupleGEMVFATStatusFiller()
{

};

void MuNtupleGEMVFATStatusFiller::initialize()
{
    m_tree->Branch((m_label + "_station").c_str(), &m_OHStatus_station);
    m_tree->Branch((m_label + "_region").c_str(), &m_OHStatus_region);
    m_tree->Branch((m_label + "_chamber").c_str(), &m_OHStatus_chamber);
    m_tree->Branch((m_label + "_layer").c_str(), &m_OHStatus_layer);
    m_tree->Branch((m_label + "_VFATMasked").c_str(), &m_OHStatus_VFATMasked);
    m_tree->Branch((m_label + "_VFATZS").c_str(), &m_OHStatus_VFATZS);
}

void MuNtupleGEMVFATStatusFiller::clear()
{
    m_OHStatus_region.clear();
    m_OHStatus_station.clear();
    m_OHStatus_chamber.clear();
    m_OHStatus_layer.clear();
    m_OHStatus_VFATMasked.clear();
    m_OHStatus_VFATZS.clear();
}

void MuNtupleGEMVFATStatusFiller::fill(const edm::Event &ev)
{

    clear();

    const auto gem = m_config->m_gemGeometry;
    edm::Handle<GEMOHStatusCollection> OH_StatusCollection;
    ev.getByToken(m_gemOHStatusToken, OH_StatusCollection);

    if (OH_StatusCollection.isValid())
    {
        for (auto ohIt = OH_StatusCollection->begin(); ohIt != OH_StatusCollection->end(); ohIt++)
        {
            const GEMDetId &gem_id = (*ohIt).first;
            const GEMOHStatusCollection::Range &range = (*ohIt).second;

            int region = gem_id.region();
            int station = gem_id.station();
            int chamber = gem_id.chamber();
            int layer = gem_id.layer();

            for (auto OHStatus = range.first; OHStatus != range.second; ++OHStatus)
            {

                const uint32_t vfatMask = OHStatus->vfatMask();
                const uint32_t zsMask = OHStatus->zsMask();
                // const uint32_t missingVFATs = OHStatus->missingVFATs();

                // skip if all VFATs are valid i.e. vfatMask = FFFFFF
                if (vfatMask == 16777215)
                {
                    continue;
                }

                else
                {
                    std::bitset<24> vfatMaskbits(vfatMask);
                    std::bitset<24> zsMaskbits(zsMask);
                    const auto zsString = zsMaskbits.to_string();

                    short MaskedVFAT = -2;
                    short ZSVFAT = -2;


                    // std::bitset<24> missingVFATbits(missingVFATs);
                    // std::cout<<"re: "<<region<<"\tch: "<<chamber<<"\tly: "<<layer<<"\tVFATMask: "<<vfatMaskbits<<std::endl;
                    // std::cout<<"re: "<<region<<"\tch: "<<chamber<<"\tly: "<<layer<<"\tVFATMask: "<<vfatMaskbits<<"\tzsMask: "<<zsMaskbits.to_string()<<"\tmissingVFAT: "<<missingVFATbits.to_string()<<std::endl;

                    // TODO: Improve conversion and vector filling
                    int VFAT_Pos = 23;
                    for (auto i : vfatMaskbits.to_string())
                    {                        
                        if (i == '0')  MaskedVFAT = VFAT_Pos;                                                      
                        else MaskedVFAT = -1;

                        if (zsString[23-VFAT_Pos] == '1') ZSVFAT = VFAT_Pos;
                        else ZSVFAT = -1;

                        m_OHStatus_station.push_back(station);
                        m_OHStatus_region.push_back(region);
                        m_OHStatus_chamber.push_back(chamber);
                        m_OHStatus_layer.push_back(layer);
                        // TODO: double check for VFAT position
                        m_OHStatus_VFATMasked.push_back(MaskedVFAT);
                        m_OHStatus_VFATZS.push_back(ZSVFAT);

                        VFAT_Pos--;
                    } // Loop over VFAT Postions
                }     // At least 1 masked VFAT
            }         // Loop over the OHStatus
        }             // Loop through the collection
    }                 // OH_StatusCollection is valid

    else
    {
        std::cout << "OH_Statuscollection is invalid" << std::endl;
    }

    return;
}
