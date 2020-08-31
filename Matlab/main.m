%% Configuration
clear all;
close all;

% Path to ns-3 simulation results folder
rootDir = "/ns-3-dev/simulation-results/";
% Number of experiments
numExp = 20;
% Folder containing the simulation results
nameExp = "fullVersion";
% Feeders to be investigated
feederIDs = 1:36;

scenarioFiles = repmat([nameExp],1,numExp);
%% 450 Urban
configurationFiles = ["LowUrban-.1","LowUrban-.2","LowUrban-.3","LowUrban-.4","LowUrban-.5","LowUrban-.6","LowUrban-.7","LowUrban-.8","LowUrban-.9","LowUrban-1.0",...
                      "LowUrban-1.1","LowUrban-1.2","LowUrban-1.3","LowUrban-1.4","LowUrban-1.5","LowUrban-1.6","LowUrban-1.7","LowUrban-1.8","LowUrban-1.9","LowUrban-2.0"];
saveName = "Urban450.mat";
processRawSimulationFiles(rootDir, scenarioFiles, configurationFiles, saveName, 1, feederIDs)

%% 2100 Urban
configurationFiles = ["HighUrban-.1","HighUrban-.2","HighUrban-.3","HighUrban-.4","HighUrban-.5","HighUrban-.6","HighUrban-.7","HighUrban-.8","HighUrban-.9","HighUrban-1.0",...
                      "HighUrban-1.1","HighUrban-1.2","HighUrban-1.3","HighUrban-1.4","HighUrban-1.5","HighUrban-1.6","HighUrban-1.7","HighUrban-1.8","HighUrban-1.9","HighUrban-2.0"];
saveName = "Urban2100.mat";
processRawSimulationFiles(rootDir, scenarioFiles, configurationFiles, saveName, 1, feederIDs)

%% 450 Rural
configurationFiles = ["LowRural-.1","LowRural-.2","LowRural-.3","LowRural-.4","LowRural-.5","LowRural-.6","LowRural-.7","LowRural-.8","LowRural-.9","LowRural-1.0",...
                      "LowRural-1.1","LowRural-1.2","LowRural-1.3","LowRural-1.4","LowRural-1.5","LowRural-1.6","LowRural-1.7","LowRural-1.8","LowRural-1.9","LowRural-2.0"];
saveName = "Rural450.mat";
processRawSimulationFiles(rootDir, scenarioFiles, configurationFiles, saveName, 1, feederIDs)

%% 2100 Rural
configurationFiles = ["HighRural-.1","HighRural-.2","HighRural-.3","HighRural-.4","HighRural-.5","HighRural-.6","HighRural-.7","HighRural-.8","HighRural-.9","HighRural-1.0",...
                      "HighRural-1.1","HighRural-1.2","HighRural-1.3","HighRural-1.4","HighRural-1.5","HighRural-1.6","HighRural-1.7","HighRural-1.8","HighRural-1.9","HighRural-2.0"];
saveName = "Rural2100.mat";
processRawSimulationFiles(rootDir, scenarioFiles, configurationFiles, saveName, 1, feederIDs)
