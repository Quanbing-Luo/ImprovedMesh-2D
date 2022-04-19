clear;

fid = fopen('Mesh.txt');
string1=fgets(fid);
ns = fscanf(fid,'%i',1);
%ps= fscanf(fid,'%*i %f %f \n',[2 ns]);
M= fscanf(fid,'%*i %f %f %f\n',[3 ns]);
ps=M(1:2,:); rs=M(3,:);
%rs=1.7*rs;


string2=fgets(fid);
nts = fscanf(fid,'%i',1);
ts = fscanf(fid,'%*i %i %i %i \n',[3 nts])+1;

% string2=fgets(fid);
% nts = fscanf(fid,'%i',1);
% ts=fscanf(fid,'%*i %i %i %i \n',[3 nts])+1;
% % MM = fscanf(fid,'%*i %i %i %i %f\n',[4 nts]);
% % ts=MM(1:3,:)+1; rrs=MM(4,:);

fclose(fid);

figure (1); clf;
pdemesh(ps,ts);
%pdemesh(nodes,elements,'ElementLabels','on');
%pdemesh(ps,ts,'NodeLabels','on');
% xlabel('$x$','interpreter','latex');
% ylabel('$y$','interpreter','latex');
% xlim([-1 1]);
% ylim([-1 1]);
% xticks([-1 0 1]);yticks([-1 0 1]);

hold on  
for i=1:length(rs)
rectangle('Position',[ps(1,i)-rs(i),ps(2,i)-rs(i),2*rs(i),2*rs(i)], ...
    'Curvature', [1 1], 'EdgeColor','r');
end


axis off equal

xs=ps(1,:);
ys=ps(2,:);
t1=ts(1,:);
t2=ts(2,:);
t3=ts(3,:);

la=hypot(xs(t2)-xs(t1), ys(t2)-ys(t1));
lb=hypot(xs(t3)-xs(t1), ys(t3)-ys(t1));
lc=hypot(xs(t3)-xs(t2), ys(t3)-ys(t2));
l=(la+lb+lc)/2;
area=sqrt(2*la.^2.*lb.^2+2*la.^2.*lc.^2+2*lb.^2.*lc.^2 ...
    - la.^4 - lb.^4 - lc.^4 )./4;
area_t=abs((xs(t2)-xs(t1)).*(ys(t3)-ys(t1))-(xs(t3)-xs(t1)).*(ys(t2)-ys(t1)))/2 ;

r=area./l;
R=(la.*lb.*lc)./(4*area);
rho=2*r./R;

figure (3);clf;
histogram(rho,0:0.1:1,'Normalization','probability');
set(gcf,'Position',[230 250 400 400]) ;
set(gca,'Position',[0.12 0.14 0.85 0.8]);
xlim([0 1]);
xlabel('mesh quality (radius ratio)','interpreter','latex');
ylabel('Percentage','interpreter','latex');
grid on

