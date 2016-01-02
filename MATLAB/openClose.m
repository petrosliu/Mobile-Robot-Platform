function imgn = openClose( img,option,num )
    imgn=img;
    for k=2:num
        se=strel('disk',k);
        switch option
            case 'o'
                imgn=imopen(imgn,se);
            case 'c'
                imgn=imclose(imgn,se);
            otherwise
                imgn=imclose(imopen(imgn,se),se);
        end
    end
end