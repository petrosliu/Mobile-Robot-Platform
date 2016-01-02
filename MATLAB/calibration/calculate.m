%Kinect Depth Calibration

s=zeros(640,480);
s1=0;
fname='dep_a';
for i = 1:25
    %num=0;
    
    buf1=readDepth(fname);
    s1=s1+median(median(buf1));
    %for m=100:640
    %    for n=1:300
    %        %read image file
    %                   
    %    end
    %    
    %end
    fname(5)=char(fname(5)+1);
    fname
end
s1=s1/25;
s1