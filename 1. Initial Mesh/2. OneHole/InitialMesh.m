clear;


N1=20; dd1=1/20;
x1=[(0.5-(0:(N1-1))*dd1), (-0.5*ones(1,N1)),  (-0.5+(0:(N1-1))*dd1),  (0.5*ones(1,N1)) ];
y1=[(0.5*ones(1,N1)),  (0.5-(0:(N1-1))*dd1),  (-0.5*ones(1,N1)),   (-0.5+(0:(N1-1))*dd1) ];
n1=size(x1,2);
r1=ones(size(x1))*dd1/2;

N2=25;r=0.1/2; 
dx=(0.5-2*r)*2/3+r;  dy=(0.55-2*r)/2+r;
theta=2*pi*(0:(N2-1))/N2;
rr=hypot(r*cos(2*pi/N2)-r, r*sin(2*pi/N2))/2;

x2=r*cos(theta);
y2=r*sin(theta);
n2=size(x2,2)+n1;
r2=ones(size(x2))*rr;

% x=[x1,x2,x3,x4,x5];
% y=[y1,y2,y3,y4,y5];

ps=[x1,x2;y1,y2];
rs=[r1,r2];

es=[1:n2; 2:n1,1, (n1+2):n2,(n1+1)];


DT = delaunayTriangulation(ps',es');
TF = isInterior(DT);
ts=(DT.ConnectivityList(TF,:))';

figure (1);clf;

%triplot(DT);
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



