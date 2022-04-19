clear;
clf;

N1=20; dd1=1/20;
x1=[(0.5-(0:(N1-1))*dd1), (-0.5*ones(1,N1)),  (-0.5+(0:(N1-1))*dd1),  (0.5*ones(1,N1)) ];
y1=[(0.5*ones(1,N1)),  (0.5-(0:(N1-1))*dd1),  (-0.5*ones(1,N1)),   (-0.5+(0:(N1-1))*dd1) ];
n1=size(x1,2);
r1=ones(size(x1))*dd1/2;

N2=round(0.35/dd1);
dd2=0.35/N2;
v1=(0:(N2-1))*dd2; v2=ones(1,N2);

x2=[(0.4-v1),  (0.05*v2),  (0.05+v1),  (0.4*v2)];
y2=[(0.4*v2),  (0.4-v1) ,  (0.05*v2),  (0.05+v1)];
n2=size(x2,2)+n1;
r2=ones(size(x2))*dd2/2;

x3=[(-0.4+v1),  (-0.05*v2), (-0.05-v1),  (-0.4*v2)];
y3=[(-0.4*v2),  (-0.4+v1),  (-0.05*v2), (-0.05-v1)];
n3=size(x3,2)+n2;
r3=ones(size(x3))*dd2/2;

x4=[(0.4-v1),  (0.05*v2),  (0.05+v1),   (0.4*v2) ];
y4=[(-0.4*v2), (-0.4+v1),  (-0.05*v2),  (-0.05-v1) ];
n4=size(x4,2)+n3;
r4=ones(size(x4))*dd2/2;

x5=[(-0.4+v1),  (-0.05*v2), (-0.05-v1),  (-0.4*v2)];
y5=[(0.4*v2),   (0.4-v1),   (0.05*v2),   (0.05+v1)];
n5=size(x5,2)+n4;
r5=ones(size(x5))*dd2/2;

% x=[x1,x2,x3,x4,x5];
% y=[y1,y2,y3,y4,y5];

ps=[x1,x2,x3,x4,x5;y1,y2,y3,y4,y5];
rs=[r1,r2,r3,r4,r5];

es=[1:n5; 2:n1,1, (n1+2):n2,(n1+1), (n2+2):n3, (n2+1), (n3+2):n4,(n3+1), (n4+2):n5,(n4+1)];


DT = delaunayTriangulation(ps',es');
TF = isInterior(DT);
ts=(DT.ConnectivityList(TF,:))';

figure (1)
pdemesh(ps,ts);

hold on
for i=1:length(rs)
rectangle('Position',[ps(1,i)-rs(i),ps(2,i)-rs(i),2*rs(i),2*rs(i)], ...
    'Curvature', [1 1], 'EdgeColor','r');
end
axis equal off



fileID = fopen('InitialMesh.txt','w');
fprintf(fileID,'Points\r\n');
np=size(ps,2);
fprintf(fileID,'%i \r\n',np);
fprintf(fileID,'%i \t %e \t %e \t %e \r\n', [1:np; ps; rs]);

fprintf(fileID,'Triangles\r\n');
nt=size(ts,2);
fprintf(fileID,'%i \r\n',nt);
fprintf(fileID,'%i \t %i \t %i \t %i \t \r\n', [1:nt ; (ts-1)]);
fclose(fileID);



