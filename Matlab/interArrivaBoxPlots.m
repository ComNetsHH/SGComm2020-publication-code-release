%% Load Data
clear all
close all

% Load Data
rootDir = "/ns-3-dev/simulation-results/fullVersion/Statistics/";

% Switch between Scenarios
%% Urban
importFiles = ["Urban450.mat";"Urban2100.mat"];
%% Rural
importFiles = ["Rural450.mat";"Rural2100.mat"];

% Add values from analyticalModel
%% Urban
analyticalModel = [0.58;0.81];
%% Rural
analyticalModel = [0.60;0.92];
%%
analyticalModel = (analyticalModel)*10;
analyticalModel = ceil(analyticalModel)-0.5;
dataCells = cell(1,size(importFiles,1));
for i = 1:size(importFiles,1)
    dataCells(i) = {load(strcat(rootDir, importFiles(i)))};
end

% FeederData has format (experiment, feeder, file, node)
%% Process
iitData = [];
iitMeans = [];
iitStd = [];
iitCells = {};
iitMega = [];
for i = 1:size(dataCells,2)
    feederData = dataCells{1,i}.feederData; 

    for experimentID = 1:size(feederData,1)
        iitData(experimentID,1:size([[vertcat(feederData(experimentID,:,:,:).InterArrivalTime)]],1)) = [[vertcat(feederData(experimentID,:,:,:).InterArrivalTime)]];
        iitData(experimentID,[[iitData(experimentID,:)] == 0]) =  NaN; 
        iitDataNoNans = (iitData(experimentID,[~isnan(iitData(experimentID,:))]));
        iitMeans(i,experimentID) = mean(iitDataNoNans);
        iitStd(i,experimentID) = std(iitDataNoNans); 
    end
    iitCells{i} = {iitData, iitMeans, iitStd};
    iitMega(:,:,i) = iitData(:,:)';
end  
%% Plot
f1 = figure;
cmap = get(gca,'colororder');

hold on
lines = [];
numX = 6;
x = {'0.1','0.2','0.3','0.4','0.5','0.6','0.7','0.8','0.9','1.0','1.1','1.2','1.3','1.4','1.5','1.6','1.7','1.8','1.9','2.0'};
xx = 0:0.1:numX+0.5-0.2;
yyy = 0.1:0.1:2;
yy = yyy(max(0,floor(xx-0.5))+1);
yy(end+1) = 2;
xx(end+1) = xx(end)+1;
figh = iosr.statistics.boxPlot(x(1:numX),iitMega(:,1:numX,:),...
          'notch',true,...
          'medianColor','auto',...
          'symbolMarker',{'+','o'},...
          'boxcolor','auto',...
          'style','normal',...
          'xSeparator',true,...
          'showViolin', false,...
          'boxWidth', 'auto',...
          'showOutliers',false,...
          'violinColor', 'auto',...
          'boxAlpha', 0.01,...
          'lineColor', 'auto',...
          'showLegend', true,...
          'groupLabels',{{'450 MHz', '2100 MHz'}});
hold on;
lines = [figh.Handles.medianLines(1,1,1), figh.Handles.medianLines(1,1,2), scatter(-10,-10,30, 'filled', '>','MarkerFaceColor',[0 0 0])];
box on;
stairs(xx,yy,'color', 0.5+zeros(1,3));
ylim([0.0,2.2]);
xlim([0.5,15.5])
hold on
%%
line([analyticalModel(1), analyticalModel(1)],[0,5], 'LineWidth', 2, 'color', cmap(1,:));
line([analyticalModel(2), analyticalModel(2)],[0,5], 'LineWidth', 2, 'color', cmap(2,:));

ar = annotation('arrow');
ar.Parent = gca;
ar.X = [analyticalModel(1) analyticalModel(1)+0.75];
ar.Y = [2   2];
ar.HeadStyle ='plain';
ar.LineWidth = 2;
ar.HeadWidth = 6;
ar.HeadLength = 6;
ar.Color = cmap(1,:);

ar = annotation('arrow');
ar.Parent = gca;
ar.X = [analyticalModel(2) analyticalModel(2)+0.75];
ar.Y = [2   2];
ar.HeadStyle ='plain';
ar.LineWidth = 2;
ar.HeadWidth = 6;
ar.HeadLength = 6;
ar.Color = cmap(2,:);

%%
xlabel("Update Interval [s]",'Position',[ceil(numX/2)-1.5 -0.25])
ylabel("Inter-Arrival Time [s]");
set(gcf,'Color',[1 1 1]);
legend(lines, ["450 MHz", "2100 MHz"]);
set(legend(), 'location','southeast')

%% Save
% paper format
paperSize = [15 5];  
paperPosition = [0.15 0.3 paperSize];      %Position plot at left hand corner with width x and height y.
% save plot
set(gcf, 'PaperUnits', 'centimeters'); %Units used for PaperSize and PaperPosition
set(gcf, 'PaperPosition', paperPosition);
set(gcf, 'PaperSize', paperSize);

%%
saveas(gcf, 'Figures/interArrialBoxPlotsUrban.pdf', 'pdf') 
%%
saveas(gcf, 'Figures/interArrialBoxPlotsRural.pdf', 'pdf') 
