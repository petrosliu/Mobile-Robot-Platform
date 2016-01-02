function rgb = readRGB( filename )
    frgb=fopen(filename);
    rgbdata=fread(frgb,921600,'uint8');
    fclose(frgb);
    rgb=uint8(zeros(640,480,3));
    for a=0:1:639          %height
        for b=0:1:479      %width
                rgb(a+1,b+1,1)=uint8(rgbdata(b*1920+a*3+1));
                rgb(a+1,b+1,2)=uint8(rgbdata(b*1920+a*3+2));
                rgb(a+1,b+1,3)=uint8(rgbdata(b*1920+a*3+3));
        end
    end
end

