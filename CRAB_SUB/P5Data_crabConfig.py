import sys
import os
import CRABClient
from WMCore.Configuration import Configuration 
from CRABAPI.RawCommand import crabCommand
import argparse
from argparse import RawTextHelpFormatter
from pathlib import Path
baseDirectory = Path(__file__).parents[1]
baseDirectory = os.path.abspath(baseDirectory)
print (baseDirectory)


parser = argparse.ArgumentParser(
        description='''Scripts that: \n\t-Submit jobs for making GEMCommonNtuples''',
        epilog="""Typical exectuion\n\t python3  P5Data_crabConfig.py  --RunList 348773 --Dataset Express""",
        formatter_class=RawTextHelpFormatter
)
parser.add_argument('--RunList','-rl', type=int,help="run(s) to be ntuplized, space separated",required=True,nargs='*')
parser.add_argument('--Dataset','-d',nargs='?',choices=['Express', 'Prompt','ZeroBias','SingleMuon','MinimumBias'],help='Dataset to be used (check availability on DAS)',required=True)

args = parser.parse_args()
if args.Dataset == 'Express':
    inputDataset= '/ExpressPhysics/Run2022B-Express-v1/FEVT'
    globalTag = ['123X_dataRun3_Express_v6']
    unitsPerJob = 20
elif args.Dataset == 'Prompt':
    globalTag = '123X_dataRun3_Prompt_v8'
    inputDataset = ['/Commissioning/Run2022A-PromptReco-v1/AOD']
    unitsPerJob = 20
elif args.Dataset == 'ZeroBias':
    globalTag = '123X_dataRun3_Prompt_v8'
    inputDataset = ['/ZeroBias'+str(i)+'/Run2022B-PromptReco-v1/AOD' for i in range(20)]
    unitsPerJob = 1
elif args.Dataset == 'SingleMuon':
    globalTag = '123X_dataRun3_Prompt_v8'
    inputDataset = ['/SingleMuon/Run2022A-PromptReco-v1/AOD']
    unitsPerJob = 1
elif args.Dataset == 'MinimumBias':
    globalTag = '123X_dataRun3_Prompt_v8'
    inputDataset = ['/MinimumBias'+str(i)+'/Run2022A-PromptReco-v1/AOD' for i in range(10)]
    unitsPerJob = 1

config = Configuration()

config.section_("General")
config.General.workArea = "{}/CRAB_SUB/".format(baseDirectory)
config.General.transferOutputs = True
config.General.transferLogs = True

config.section_("JobType")
config.JobType.pluginName = 'Analysis'
config.JobType.psetName = "{}/test/muDpgNtuples_cfg.py".format(baseDirectory)
config.JobType.allowUndistributedCMSSW = True
config.JobType.pyCfgParams = ['isMC=False','nEvents=-1','globalTag='+str(globalTag)]
config.JobType.maxMemoryMB = 5000

config.section_("Data")
config.Data.inputDBS = 'global'
config.Data.splitting = 'FileBased'
config.Data.unitsPerJob = unitsPerJob
config.Data.publication = False
config.Data.outLFNDirBase = '/store/group/dpg_gem/comm_gem/P5_Commissioning/2022/GEMCommonNtuples'
config.Data.allowNonValidInputDataset = True

config.section_("Site")
config.Site.storageSite = 'T2_CH_CERN'


run_list = args.RunList


print("Submitting runs= ",run_list)
print("Dataset = " ,inputDataset)
print("GlobalTag = ",globalTag)
print(config)


for run_number in run_list:
    for indataset in inputDataset:
        outputName = str(run_number)+'_'+str(args.Dataset)
        crab_folderName = str(run_number)+'_'+indataset.split("/")[1]
        config.Data.inputDataset = indataset
        config.Data.outputDatasetTag = outputName
        config.General.requestName = crab_folderName
        config.Data.runRange = str(run_number)
        crabCommand('submit', config = config)
sys.exit(0)
