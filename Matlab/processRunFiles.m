function nodeEntries = processRunFiles(folderDir)
infoRx = loadPacketsCSV([folderDir '/packetsRx.csv']);
infoTx = loadPacketsTxCSV([folderDir '/packetsTx.csv']);
macDl = loadDlMacStatsCSV([folderDir '/DlMacStats.txt']);
macUl = loadDlMacStatsCSV([folderDir '/UlMacStats.txt']);
phyUl = loadUlRxPhyStatsCSV([folderDir '/UlRxPhyStats.txt']);
%phyDl = loadDlRxPhyStatsCSV([folderDir '/DlRxPhyStats.txt']);
nodePos = loadPositionCSV([folderDir '/addressLocations.csv']);
sinrUl = loadUlSinrCSV([folderDir '/UlSinrStats.txt']);


macDl = macDl(table2array(macDl(:,7))>0,:);
macUl = macUl(table2array(macUl(:,7))>0,:);
temp1 = char(table2array(infoRx(:,2)));
infoRx(:,2) = array2table(string(temp1(:,2:end)));
eNodeBPos = {};
eNodeBPos.X = max(table2array(nodePos(end,3)));
eNodeBPos.Y = max(table2array(nodePos(end,4)));

nodeInfo = [];
for user = 1:size(nodePos,1)-1
    nodeInfoEntry = {};
    node = table2array(nodePos(user,1));
    transmissionsSent = [];
    transmissionsRcvd = [];
   % find all destination transmission
    transmissionIds = find(table2array(infoRx(:,2)) == node);
    transmissionsSent = find(table2array(infoTx(:,1)) == node);

    %transmissions = zeros(size(transmissionIds,1));
    if(~isempty(transmissionIds))
        transmissionsRcvd = infoRx(transmissionIds,:);
    end

    % Node Info Entry
    nodeInfoEntry.Id = user;
    nodeInfoEntry.IMSI = table2array(nodePos(user,2));
    nodeInfoEntry.CellID = table2array(phyUl(find(table2array(phyUl(:,3)) == nodeInfoEntry.IMSI,1),2));

    nodeInfoEntry.X = table2array(nodePos(user,3));
    nodeInfoEntry.Y = table2array(nodePos(user,4));
    nodeInfoEntry.FeederID = table2array(nodePos(user,5));
    nodeInfoEntry.NodeId = table2array(nodePos(user,6));

    nodeInfoEntry.Distance = sqrt((nodeInfoEntry.X-eNodeBPos.X)^2 + (nodeInfoEntry.Y-eNodeBPos.Y)^2);
    nodeInfoEntry.Mac = node;
    nodeInfoEntry.AverageDlCQI = max(0,mean(table2array(macDl(find(table2array(macDl(:,3)) == nodeInfoEntry.IMSI),7))));
    nodeInfoEntry.AverageUlCQI = max(0,mean(table2array(macUl(find(table2array(macUl(:,3)) == nodeInfoEntry.IMSI),7))));
    DlTransmissions = (table2array(phyUl(find(table2array(phyUl(:,3)) == nodeInfoEntry.IMSI),10)));
    nodeInfoEntry.AverageDlMacPRR = max(0,sum(DlTransmissions)/size(DlTransmissions,1));

    DlTransmissions = (table2array(phyUl(find(table2array(phyUl(:,3)) == nodeInfoEntry.IMSI),8)));
    nodeInfoEntry.AverageHarqStageDl = max(0,mean(DlTransmissions));

    if(contains(node,"04-07-07"))
        nodeInfoEntry.Direction = "DL";
        nodeInfoEntry.AverageUlSINR = NaN;
    else
        nodeInfoEntry.Direction = "UL";
        nodeInfoEntry.AverageUlSINR = max(0,mean(table2array(sinrUl(find(table2array(sinrUl(:,3)) == nodeInfoEntry.IMSI),5))));
    end
    nodeInfoEntry.NumPktsRcvd = size(transmissionsRcvd,1);
    nodeInfoEntry.NumPktsRcvdExpected = size(transmissionsSent,1);
    nodeInfoEntry.AverageAppPRR = nodeInfoEntry.NumPktsRcvd / nodeInfoEntry.NumPktsRcvdExpected;

    if(~isempty(transmissionsRcvd))
        N = size(table2array(transmissionsRcvd(:,4)),1);
        nodeInfoEntry.MeanE2EDelay = mean(table2array(transmissionsRcvd(:,4)));
        nodeInfoEntry.SemE2EDelay = std(table2array(transmissionsRcvd(:,4)))/sqrt(N);
    else
        nodeInfoEntry.MeanE2EDelay = [];
        nodeInfoEntry.SemE2EDelay = [];
    end
    nodeInfoEntry.Transmissions = transmissionsRcvd;  
    if(~isempty(transmissionsRcvd))
        nodeInfoEntry.InterArrivalTime = mean(max(0,diff(table2array(transmissionsRcvd(:,3)) + table2array(transmissionsRcvd(:,4)))));
    else
        nodeInfoEntry.InterArrivalTime = [];
    end
    nodeInfo = [nodeInfo, (nodeInfoEntry)];
end

controlSum = sum([nodeInfo.NumPktsRcvd]);
assert(abs(controlSum-size(infoRx,1)) < 10); % make sure most of the messages have been parsed correctly

nodeEntries = nodeInfo;
end

