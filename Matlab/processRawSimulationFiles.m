function [] = processRawSimulationFiles(rootDir, scenarioFiles, configurationFiles, saveName, numExpectedRuns, feederUnderTest)
    %% Par Load and Process
    runData = cell(size(scenarioFiles,1),numExpectedRuns);
    parfor experimentID = 1:1:size(scenarioFiles,2)
        experimentDir = scenarioFiles(experimentID);
        configurationDir = configurationFiles(experimentID);
        % Get number of runs available
        all_files = dir(strcat(rootDir,experimentDir));
        all_dir = all_files(and([all_files(:).isdir], contains({all_files(:).name},configurationDir)));
        numRuns = min(numel(all_dir),numExpectedRuns);
        rawData = [];
        temp = {zeros(1,numRuns)};
        for fileID = 1:1:numRuns
            folderPath = strcat(rootDir,experimentDir,"/", configurationDir, "-",string(fileID));
            rawData =  {processRunFiles(convertStringsToChars(folderPath))};
            temp(fileID) = rawData;
        end
        runData(experimentID,:) = temp;
    end

    %% Process
    for experimentID = 1:1:size(runData,1)
        for fileID = 1:1:size(runData,2)
            for feederID = size(feederUnderTest,2):-1:1
                rawData = [runData{experimentID,fileID}];
                feederNodes = rawData([rawData(:).FeederID] == feederUnderTest(feederID));
                feederData(experimentID,feederID,fileID,1:size(feederNodes(string([feederNodes(:).Direction]) == "UL"),2)) = feederNodes(string([feederNodes(:).Direction]) == "UL");
            end
        end
    end
    
    if ~exist(strcat(rootDir,scenarioFiles(1),"/Statistics/"), 'dir')
        mkdir(strcat(rootDir,scenarioFiles(1),"/Statistics/"));
    end

    save(convertStringsToChars(strcat(rootDir,scenarioFiles(1),"/Statistics/",saveName)), 'feederData');
    delete(gcp('nocreate'));
end