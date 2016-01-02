function dep = readDepthNum( filename,num )
    fd=fopen(filename);
    for n=1:1:num
        depth(n,:)=fread(fd,307200,'uint16');
    end
    fclose(fd);
    
    depthdata=round(median(depth));
    for m=1:1:307200
        v=0;
        for n=1:1:num
           if depth(n,m)==2047
               v=v+1;
           end
        end
        if v>round(num/3)
           depthdata(m)=2047;
        end
    end 
    dep=reshape(depthdata,640,480);
end