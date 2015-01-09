% This script is used to create tha data from the files produced by mpi-itonic
Folder = 'OutputFiles(Grid)';
FileNames = textscan(ls(Folder),'%s');


M = length((FileNames{1}));
FileIDS = zeros(M);
Data = zeros(M,5);
Raw_data = {};
for i=1:M 
  FileIDS(i) = fopen([Folder '/' FileNames{1}{i}]);
  Raw_data{i} = textscan(FileIDS(i),'%s');
  Q = Raw_data{i}{1}{3};
  Nodes = Raw_data{i}{1}{10};
  Ppn = Raw_data{i}{1}{13};
  Time = Raw_data{i}{1}{34};
  Test = Raw_data{i}{1}{36};

  if(not(strcmpi(Test,'Passed')))
    fprintf('Test didnt Pass Check it out!!!!!!!!!!')
  end
  
  DataInfo = ['Q ',' Nodes ',' Ppn ',' Time ',' Test '];
  Data(i,:) = [str2num(Q),str2num(Nodes),str2num(Ppn),str2num(Time),strcmpi(Test,'Passed')];
  
end

save('MpiDataGrid','Data')
clear
fclose('all');
