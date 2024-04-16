#include "MuDPGAnalysis/MuonDPGNtuples/src/MuNtupleGEMAMCStatusFiller.h"

#include "FWCore/Framework/interface/Event.h"
//#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/ESHandle.h"

MuNtupleGEMAMCStatusFiller::MuNtupleGEMAMCStatusFiller(edm::ConsumesCollector &&collector,
                                                         const std::shared_ptr<MuNtupleConfig> config,
                                                         std::shared_ptr<TTree> tree, const std::string &label) : MuNtupleBaseFiller(config, tree, label)
{
    edm::InputTag & iTag = m_config->m_inputTags["gemAMCStatusTag"];
    if (iTag.label() != "none") m_gemAMCStatusToken = collector.consumes<GEMAMCStatusCollection>(iTag);
}

MuNtupleGEMAMCStatusFiller::~MuNtupleGEMAMCStatusFiller(){

};

void MuNtupleGEMAMCStatusFiller::initialize()
{
    m_tree->Branch((m_label + "_FEDId").c_str(), &m_AMCStatus_FEDId);
    m_tree->Branch((m_label + "_Slot").c_str(), &m_AMCStatus_slot);
    m_tree->Branch((m_label + "_DAVList").c_str(), &m_AMCStatus_DAV_List);
    m_tree->Branch((m_label + "_linkTO").c_str(), &m_OHStatus_linkTO);
}

void MuNtupleGEMAMCStatusFiller::clear()
{
    m_AMCStatus_FEDId.clear();
    m_AMCStatus_slot.clear();
    m_AMCStatus_DAV_List.clear();
    m_OHStatus_linkTO.clear();
}

void MuNtupleGEMAMCStatusFiller::fill(const edm::Event &ev)
{
    clear();

    auto AMC_StatusCollection = conditionalGet<GEMAMCStatusCollection>(ev, m_gemAMCStatusToken,"GEMAMCStatus");

    if (AMC_StatusCollection.isValid())
    {
        for (auto amcIt = AMC_StatusCollection->begin(); amcIt != AMC_StatusCollection->end(); ++amcIt)
        {
            Int_t fedId = (*amcIt).first;
            
            const GEMAMCStatusCollection::Range &range = (*amcIt).second;

            for (auto amc = range.first; amc != range.second; ++amc) 
            {
                m_AMCStatus_FEDId.push_back(fedId);
                m_AMCStatus_slot.push_back(amc->amcNumber());
                m_AMCStatus_DAV_List.push_back(amc->davList());
                m_OHStatus_linkTO.push_back(amc->linkTO());
            }     
        } 
    }             

    else
    {
        std::cout << "AMC_StatusCollection is invalid GEMAMCStatusFiller" << std::endl;
    }

    return;
}

