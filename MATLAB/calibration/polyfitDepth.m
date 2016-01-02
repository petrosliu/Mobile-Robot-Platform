buf1=csvread('data.txt');

res=polyfit(buf1(1,:),buf1(2,:),3);
res
y2=polyval(res,(1:8:1024));
y1=buf(2,:);
plot(x,y2,'.',x,y1,'--')