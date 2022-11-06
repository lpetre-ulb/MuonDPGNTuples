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

MuNtupleGEMVFATStatusFiller::~MuNtupleGEMVFATStatusFiller(){

};

void MuNtupleGEMVFATStatusFiller::initialize()
{
    m_tree->Branch((m_label + "_station").c_str(), &m_OHStatus_station);
    m_tree->Branch((m_label + "_region").c_str(), &m_OHStatus_region);
    m_tree->Branch((m_label + "_chamber").c_str(), &m_OHStatus_chamber);
    m_tree->Branch((m_label + "_layer").c_str(), &m_OHStatus_layer);
    m_tree->Branch((m_label + "_VFATMasked").c_str(), &m_OHStatus_VFATMasked);
    m_tree->Branch((m_label + "_VFATZS").c_str(), &m_OHStatus_VFATZS);
    m_tree->Branch((m_label + "_VFATMissing").c_str(), &m_OHStatus_VFATMissing);
    m_tree->Branch((m_label + "_errors").c_str(), &m_OHStatus_errors);
    m_tree->Branch((m_label + "warnings").c_str(), &m_OHStatus_warnings);
}

void MuNtupleGEMVFATStatusFiller::clear()
{
    m_OHStatus_region.clear();
    m_OHStatus_station.clear();
    m_OHStatus_chamber.clear();
    m_OHStatus_layer.clear();
    m_OHStatus_VFATMasked.clear();
    m_OHStatus_VFATZS.clear();
    m_OHStatus_VFATMissing.clear();
    m_OHStatus_errors.clear();
    m_OHStatus_warnings.clear();
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
            int module = 0;
            for (auto OHStatus = range.first; OHStatus != range.second; ++OHStatus)
            {

                // vfatmask == 0 ==> VFAT was masked
                // zsmaks == 1   ==> VFAT was ZeroSuppressed
                // missingVFAT ==> 1 VFAT is missing (should be there but it is not)
                // existVFATs ==> 0 VFAT exists
                const uint32_t vfatMask = OHStatus->vfatMask();
                const uint32_t zsMask = OHStatus->zsMask();
                const uint32_t missingVFATs = OHStatus->missingVFATs();
                const uint32_t existVFATs = OHStatus->existVFATs();
                const uint16_t errors = OHStatus->errors();
                const uint8_t warnings = OHStatus->warnings();
                

                std::bitset<24> vfatMaskbits(vfatMask);
                std::bitset<24> zsMaskbits(zsMask);
                std::bitset<24> missingVFATbits(missingVFATs);
                std::bitset<24> existVFATbits(existVFATs);

                // std::cout << "st:" << station << "\tre: " << region << "\tch: " << chamber << "\tly: " << module << "\tModule: " << 3 << "\n\tVFATMask: " << vfatMaskbits.to_string() << "\n\tzsMask: " << zsMaskbits.to_string() << "\n\tmissingVFAT: " << missingVFATbits.to_string() << "\n\texistVFATbits: " << existVFATbits.to_string() << std::endl;
                // std::cin.get();
                // skip if  no VFAT masked, ZS nor missing
                if (vfatMask == 16777215 && zsMask == 0 && missingVFATs == 0 && errors == 0 && warnings == 0)
                {
                    continue;
                }

                else
                {

                    m_OHStatus_station.push_back(station);
                    m_OHStatus_region.push_back(region);
                    m_OHStatus_chamber.push_back(chamber);
                    m_OHStatus_layer.push_back(layer);
                    // TODO: double check for VFAT position
                    m_OHStatus_VFATMasked.push_back(vfatMask);
                    m_OHStatus_VFATZS.push_back(zsMask);
                    m_OHStatus_VFATMissing.push_back(missingVFATs);
                    m_OHStatus_errors.push_back(errors);
                    m_OHStatus_warnings.push_back(warnings);

                } // At least 1 masked VFAT
            }     // Loop over the OHStatus
        }         // Loop through the collection
    }             // OH_StatusCollection is valid

    else
    {
        std::cout << "OH_Statuscollection is invalid" << std::endl;
    }

    return;
}
