% script to plot some 1-d average histograms
% 
% Note, if the data was standardised for each histogram, each one will
% probably have mode around 0, but the unstandardised polygons will show
% the actual differences in location
%
%
% The input file gives a list of filenames that are used to make the
% graphics, one by one
% 
% Each histogram will have the same x and y limits
%
% Each image is saved separately
% 
% All images are named with the base name prepended and 
% saved in the current folder. 

clear functions
clear variables

%put the name of the input file here
filename = 'AABCsimdata2DGR_s_10_ns_25000_th_0.02_ni_10000_Avs.txt';

%put the base name of the output files here
%this can include a path, in windows format, ie '\' path\file
outname = 'simdata2D_';


%change the parameter name
% (these are used for labelling axes)
ssName = 'theta''';

f1 = @Function1DBoxesPlot;
f2 = @FunctionGetMaxHeight;
f3 = @FunctionGetXSpread;

[f d1] = ReadFilenamesAndSize(filename);

% find the axes limits over all the files
heightindex = 2; % column index for heights
lowerXindex = 3; % column index for lower x coordinates
upperXindex = 4; % column index for upper x coordinates

maxHeight = f2(f, heightindex);
maxHeight = maxHeight*1.1; % padding
[xLower, xUpper] = f3(f, lowerXindex, upperXindex);
xLower = xLower - (xUpper - xLower) * 0.05; % padding
xUpper = xUpper + (xUpper - xLower) * 0.05;

for i=1:size(d1)
    
    boxesFileName = f{1}{i};
    
    figure;

    h1 = gca;
    
    cla(h1);

    p = f1(boxesFileName, h1);
    
    set (h1,'XLim',[xLower xUpper]);
    set(h1,'YLim',[0 maxHeight]);
    
    set(get(h1,'XLabel'),'String',ssName,'Interpreter', 'none');
   
    set(get(h1,'Title'),'String',boxesFileName,'Interpreter', 'none');
    
    %get the index for this observed stat out of the filename
    slDiv1 = regexp(boxesFileName, '_', 'split');
    slDiv2 = regexp(slDiv1{size(slDiv1,2)}, '\.txt', 'split');
    index = slDiv2{1};

    outputfile = strcat(outname, 'Posterior', index, '.png');
    
    print ('-dpng', outputfile);
end
