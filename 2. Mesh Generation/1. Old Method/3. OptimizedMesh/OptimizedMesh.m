clear;
clf;
fid = fopen('OptimizedNodes.txt');
string1=fgets(fid);
nps = fscanf(fid,'%i',1);
M= fscanf(fid,'%*i %f %f %f\n',[3 nps]);
ps=M(1:2,:);
rs=M(3,:);

string2=fgets(fid);
nts = fscanf(fid,'%i',1);
ts = fscanf(fid,'%*i %i %i %i \n',[3 nts])+1;

string3=fgets(fid);
nes = fscanf(fid,'%i',1);
es = fscanf(fid,'%*i %i %i  \n',[2 nes])+1;

fclose(fid);

%DT = delaunayTriangulation(ps');
DT = delaunayTriangulation(ps',es');
TF = isInterior(DT);
ts=(DT.ConnectivityList(TF,:))';
%ts=(DT.ConnectivityList)';

figure (1)
pdemesh(ps,ts);
%pdemesh(ps,ts,'NodeLabels','on');

% hold on
% for i=1:length(rs)
% rectangle('Position',[ps(1,i)-rs(i),ps(2,i)-rs(i),2*rs(i),2*rs(i)], ...
%     'Curvature', [1 1], 'EdgeColor','r');
% end

% hold on
% x_es=reshape(ps(1,es), size(es));
% y_es=reshape(ps(2,es), size(es));
% plot(x_es,y_es,'r');
axis off equal


fileID = fopen('OptimizedMesh.txt','w');
fprintf(fileID,'Points\r\n');
np=size(ps,2);
fprintf(fileID,'%i \r\n',np);
fprintf(fileID,'%i \t %e \t %e \t %e \r\n', [1:np;ps;rs]);

fprintf(fileID,'Triangles\r\n');
nt=size(ts,2);
fprintf(fileID,'%i \r\n',nt);
fprintf(fileID,'%i \t %i \t %i \t %i \t \r\n', [1:nt ; ts-1]);

fprintf(fileID,'Edges\r\n');
ne=size(es,2);
fprintf(fileID,'%i \r\n',ne);
fprintf(fileID,'%i \t %i \t %i  \r\n', [1:ne ; es-1]);

fclose(fileID);




