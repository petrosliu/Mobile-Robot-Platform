function dep = readDepth( filename )
    fd=fopen(filename);
    depthdata=fread(fd,307200,'uint16');
    fclose(fd);
    dep=reshape(depthdata,640,480);
end

