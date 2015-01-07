% This script is used to create tha data from the files produced by mpi-itonic
Folder = 'OutputFiles(UI)';
FileNames = textscan(ls(Folder),'%s');


M = length((FileNames{1}));
FileIDS = zeros(M);
Data = zeros(M,5);
Raw_data = {};
for i=1:M 
  FileIDS(i) = fopen([Folder '/' FileNames{1}{i}]);
  Raw_data{i} = textscan(FileIDS(i),'%s');
  Q = Raw_data{i}{1}{2};
  Nodes = '1';
  Ppn = Raw_data{i}{1}{1};
  Time = Raw_data{i}{1}{3};
  Test = 'Passed';

  if(not(strcmpi(Test,'Passed')))
    fprintf('Test didnt Pass Check it out!!!!!!!!!!')
  end
  
  DataInfo = ['Q ',' Nodes ',' Ppn ',' Time ',' Test '];
  Data(i,:) = [str2num(Q),str2num(Nodes),str2num(Ppn),str2num(Time),strcmpi(Test,'Passed')];
  
end

save('MpiData','Data')
clear
fclose('all');
