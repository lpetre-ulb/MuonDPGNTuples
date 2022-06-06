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

inputDataset = '/Cosmics/fsimone-Commissioning2022_350174_reco_v1-774cec70fb24e91a7d0e294369c07bdc/USER'
globalTag = '123X_dataRun3_Express_v6'

config = Configuration()

config.section_("General")
config.General.workArea = "{}/CRAB_SUB/".format(baseDirectory)
config.General.transferOutputs = True
config.General.transferLogs = True

config.section_("JobType")
config.JobType.pluginName = 'Analysis'
config.JobType.psetName = "{}/test/muDpgNtuples_cfg_test.py".format(baseDirectory)
config.JobType.allowUndistributedCMSSW = True
config.JobType.pyCfgParams = ['isMC=False','nEvents=-1','globalTag='+str(globalTag)]

config.section_("Data")
config.Data.inputDBS = 'phys03'
config.Data.splitting = 'FileBased'
config.Data.unitsPerJob = 2
config.Data.publication = False
config.Data.outLFNDirBase = '/store/group/dpg_gem/comm_gem/P5_Commissioning/2022/GEMCommonNtuples'
config.Data.allowNonValidInputDataset = True

config.section_("Site")
config.Site.storageSite = 'T2_CH_CERN'

run_list = ['350174']


print("Submitting runs= ",run_list)
print("Dataset = " ,inputDataset)
print("GlobalTag = ",globalTag)
print()


for run_number in run_list:
    FolderName = 'Commissioning2022_'+str(run_number)+'_'+str('GE21')
    config.Data.inputDataset = inputDataset
    config.Data.outputDatasetTag = FolderName
    config.General.requestName = FolderName
    config.Data.runRange = str(run_number)
    crabCommand('submit', config = config)
sys.exit(0)
