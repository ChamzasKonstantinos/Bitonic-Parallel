function create_digram(parallel_data,serial_data)

load(parallel_data);
Mpi_Processes =  Data(:,2).*Data(:,3)
Q = Data(:,1);
Q_Vector = [16 17 18 19 20];
Index = Q-15;
P_Time = Data(:,4);
load(serial_data);
S_Time = Data;

Time_MPI = ones(length(P_Time)/length(Q_Vector),length(Q_Vector));
Legend_vector = {};
for i=1:size(P_Time)
  Time_MPI(log2(Mpi_Processes(i)),Index(i)) = P_Time(i);
end
Time_MPI
for i=1:size(Time_MPI,1)
  Time_MPI(i,:) = S_Time(i,:)./Time_MPI(i,:);
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
legend(Legend_vector)
saveas(h,'Diagramm.jpeg')
