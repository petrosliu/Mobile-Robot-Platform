clear all;
clc;
dep=readDepthNum('dep10in1',10);
%imgn=readDepth('dep10in1');
rgb=readRGB('rgb');
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%




depoc=openClose(dep,0,5);
rgboc=openClose(rgb,0,5);
%depla=laplacianGauss(dep/8,1);
rgbla=laplacianGauss(rgboc,3);

%imgn=openClose(removeGhost(dep),0,5);
imgn=removeGhost(openClose(dep,0,5));
depla=laplacianGauss(imgn,1);
%imgm=openClose(removeGhost(dep),0,5);
%Zoom
depla_zoom=zeros(640,480);
dep_zoom=zeros(640,480);
scale_f=1.084509;
for i=1:640
    for j=1:480
        if (round(scale_f*i)<=640)&&(j-2>0)
            depla_zoom(i,j)=depla(round(scale_f*i),j-2);
            dep_zoom(i,j)=imgn(round(scale_f*i),j-2);
        else
            depla_zoom(i,j)=0;
            dep_zoom(i,j)=2047;
        end
    end
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%SB
matched=LineEnhance(rgbla,depla_zoom,dep);
dep1=matched(:,:,2);
dep2=matched(:,:,3);
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
if 0
%Zoom


%Comparison 1: rgbla and depla and depla_revised
temp(:,:,1)=rgbla;
temp(:,:,2)=depla;
temp(:,:,3)=zeros(640,480);
%%
temp(:,:,2)=circshift(depla,[-34,2]);
end
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%Comaprison 2: rgbla and matched points
temp(:,:,1)=rgbla;
temp(:,:,2)=100*matched(:,:,1);
temp(:,:,3)=0*depla_zoom;

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
figure(1);
show1=temp;
show2=depla;
subplot(1,4,1);
imshow(show1,[0,40]);
subplot(1,4,2);
imshow(show2,[0,30]);
subplot(1,4,3)
%imshow(matched(:,:,4),[0,1])
subplot(1,4,4)
%imshow(matched(:,:,1),[0,1])
figure(2);
%imshow(matched(:,:,4),[0,1])


tttttemp=openClose(fitEdge(dep_zoom,matched(:,:,2),depla_zoom),0,5);
figure(3);
subplot(1,3,1);
imshow(dep_zoom,[]);
subplot(1,3,2);
imshow(tttttemp,[]);
subplot(1,3,3);
imshow(matched(:,:,2));


%pointCloud(rgb,round(imgn/8));

imwrite(uint8(depla/max(depla)*256),'depla.bmp','bmp');

imwrite(uint8(rgb),'1.bmp','bmp');
imwrite(uint8(tttttemp/8),'2.bmp','bmp');
%imwrite(uint16(tttttemp),'2.png','png');


fid=fopen('depfindata.txt','w+');
for m=1:1:480
    for n=1:1:640
        fprintf(fid,'%d\n',tttttemp(n,m));
    end
end
fclose(fid);
fid=fopen('depdata.txt','w+');
for m=1:1:480
    for n=1:1:640
        fprintf(fid,'%d\n',dep(n,m));
    end
end
fclose(fid);
fid=fopen('matchdata.txt','w+');
for m=1:1:480
    for n=1:1:640
        fprintf(fid,'%d\n',dep_zoom(n,m));
    end
end
fclose(fid);