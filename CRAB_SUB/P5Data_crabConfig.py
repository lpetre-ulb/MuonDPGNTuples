import sys
import os
import CRABClient
from WMCore.Configuration import Configuration 
from CRABAPI.RawCommand import crabCommand
import argparse
from argparse import RawTextHelpFormatter

parser = argparse.ArgumentParser(
        description='''Scripts that: \n\t-Submit jobs for making GEMCommonNtuples''',
        epilog="""Typical exectuion\n\t python3  P5Data_crabConfig.py  --RunList 348773 --Dataset Express""",
        formatter_class=RawTextHelpFormatter
)
parser.add_argument('--RunList','-rl', type=int,help="run(s) to be ntuplized, space separated",required=True,nargs='*')
parser.add_argument('--Dataset','-d',nargs='?',choices=['Express', 'Prompt'],help='Dataset to be used (check availability on DAS)',required=True)

args = parser.parse_args()
if args.Dataset == 'Express':
    inputDataset = '/ExpressCosmics/Commissioning2022-Express-v1/FEVT'
    globalTag = '122X_dataRun3_Express_v3'
elif args.Dataset == 'Prompt':
    globalTag = '122X_dataRun3_Prompt_v3'
    inputDataset = '/Cosmics/Commissioning2022-PromptReco-v1/AOD'

config = Configuration()

config.section_("General")
config.General.workArea = "/afs/cern.ch/user/f/fivone/Documents/NTuplizer/CRUZET/CMSSW_12_2_1/src/MuDPGAnalysis/"
config.General.transferOutputs = True
config.General.transferLogs = True

config.section_("JobType")
config.JobType.pluginName = 'Analysis'
config.JobType.psetName = '/afs/cern.ch/user/f/fivone/Documents/NTuplizer/CRUZET/CMSSW_12_2_1/src/MuDPGAnalysis/MuonDPGNtuples/test/muDpgNtuples_cfg.py'
config.JobType.allowUndistributedCMSSW = True
config.JobType.pyCfgParams = ['isMC=False','nEvents=-1','globalTag='+str(globalTag)]

config.section_("Data")
config.Data.inputDBS = 'global'
config.Data.splitting = 'Automatic'
config.Data.unitsPerJob = 180
config.Data.publication = False
config.Data.outLFNDirBase = '/store/group/dpg_gem/comm_gem/P5_Commissioning/2022/GEMCommonNtuples'
#config.Data.outLFNDirBase = '/store/user/fivone/GEMMuonNtuplizerZmumu'
config.Data.allowNonValidInputDataset = True

config.section_("Site")
config.Site.storageSite = 'T2_CH_CERN'
#config.Site.storageSite = 'T2_DE_RWTH'

#config.section_("User")
#config.User.voGroup = 'dcms'

run_list = args.RunList


print("Submitting runs= ",run_list)
print("Dataset = " ,inputDataset)
print("GlobalTag = ",globalTag)
print()


for run_number in run_list:
    FolderName = str(run_number)+'_'+str(args.Dataset)
    config.Data.inputDataset = inputDataset
    config.Data.outputDatasetTag = FolderName
    config.General.requestName = FolderName
    config.Data.runRange = str(run_number)
    crabCommand('submit', config = config)
sys.exit(0)
