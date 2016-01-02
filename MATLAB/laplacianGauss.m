function imgn = laplacianGauss( img,page )
    w=[-2,-4,-4,-4,-2; ...
       -4, 0, 8, 0,-4; ...
       -4, 8,24, 8,-4; ...
       -4, 0, 8, 0,-4; ...
       -2,-4,-4,-4,-2];
   for n=1:page
       pagela(:,:,n)=imfilter(uint8(img(:,:,n)),w,'replicate');
   end
   imgn=uint8(zeros(640,480));
   for n=1:page
       imgn=imgn+pagela(:,:,n)/page;
   end
   imgn=medfilt2(medfilt2(medfilt2(imgn,[3,3]),[3,3]),[3,3]);
end


