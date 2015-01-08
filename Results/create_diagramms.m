function create_digram(parallel_data,serial_data,name)


%Create Parallel Data
load(parallel_data);
Mpi_Processes =  Data(:,2).*Data(:,3);
Q = Data(:,1);
Q_Vector = [16 17 18 19 20];
Index = Q-15;
P_Time = Data(:,4);

Time_MPI = ones(length(P_Time)/length(Q_Vector),length(Q_Vector));
Legend_vector = {};
for i=1:size(P_Time)
  Time_MPI(log2(Mpi_Processes(i)),Index(i)) = P_Time(i);
end
Time_MPI


%Create Serial Data
load(serial_data);
Mpi_Processes =  Data(:,2).*Data(:,3);
Q = Data(:,1);
Q_Vector = [16 17 18 19 20];
Index = Q-15;
S_Time = Data(:,4);

Time_Serial = ones(length(S_Time)/length(Q_Vector),length(Q_Vector));
for i=1:size(S_Time)
  Time_Serial(log2(Mpi_Processes(i)),Index(i)) = S_Time(i);
end
Time_Serial

for i=1:size(Time_MPI,1)
  Time_MPI(i,:) = Time_Serial(i,:)./Time_MPI(i,:);
end

for i=1:size(Time_MPI,1)
  Legend_vector{i} =['MPI-Tasks ',num2str(2^i)];
end
Time_MPI

h = figure(1);
 
save('Serial_Mpi_Time','Time_MPI')
plot(Time_MPI')
set(gca, 'XTick',1:5, 'XTickLabel',Q_Vector)
xlabel('Q')
ylabel('serial/parallel')
legend(Legend_vector,'Location','Best')

grid
if((size(name))>1)
  saveas(h,'Diagramm.jpeg')
else
  title(name)
  saveas(h,[name '.png'])
end
