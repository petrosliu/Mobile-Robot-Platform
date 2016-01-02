function dep_new = LineEnhance (rgbla,depla,dep)
    rgbsymbol=zeros(640,480);
    depsymbol=zeros(640,480,2);
    for Ax1=2:639
        for Ay1=2:479
            %find the edge of dep
            if depla(Ax1,Ay1)>=10
                 if rgbla(Ax1,Ay1)>=35%yu zhi
                     rgbsymbol(Ax1,Ay1)=1;
                 end
            end
                
        end
    end
    rgblineup(:,:,1)=rgbsymbol;
    rgblineup(:,:,2)=zeros();

    for Ax1=2:639
        for Ay1=2:479
            %line up
            if rgblineup(Ax1,Ay1,1)==1 && rgblineup(Ax1,Ay1,2)==0;%Judgement of if the point is marked and unprocessed
                for m=-1:2:1
                    for n=-1:2:1
                        k=1;
                            while rgbla(Ax1+k*m,Ay1+k*n)>=20 ...
                                &&  rgblineup(Ax1+k*m,Ay1+k*n,1)==1 &&   rgblineup(Ax1+k*m,Ay1+k*n,2)==0
                                
                                k=k+1;
                                
                                if Ax1+k*m>640 || Ay1+k*n >480 || Ax1+k*m<=0 || Ay1+k*n<=0
                                break;
                                end
                            end
                        if k>=4
                            
                            for ls=1:k-1      %ls,length of stretch
                                rgblineup(Ax1+ls*m,Ay1+ls*n,:)=[1,1];
                            end
                        end
                    end
                end                            
            end    
        end
    end
    if 0%Second enhancement
    rgblineup(Ax1,Ay1,2)=rgblineup(Ax1,Ay1,1);
    for Ax1=2:639
        for Ay1=2:479
            if rgblineup(Ax1,Ay1,1)==1
            for m=-1:2:1
                    for n=-1:2:1
                        k=1;
                            while rgblineup(Ax1+k*m,Ay1+k*n,1)==1
                                
                                k=k+1;
                                
                                if Ax1+k*m>640 || Ay1+k*n >480 || Ax1+k*m<=0 || Ay1+k*n<=0
                                break;
                                end
                            end
                            lbuf=0;
                            if k>=3
                            for ls=0:round(1.2*k)     %ls,length of stretch
                                if Ax1+(ls+k)*m>640 || Ay1+(ls+k)*n >480 || Ax1+(ls+k)*m<=0 || Ay1+(ls+k)*n<=0
                                break;
                                end
                                if rgblineup(Ax1+(ls+k)*m,Ay1+(ls+k)*n,1)==1
                                    lbuf=ls+lbuf;%Save the passed length
                                    ls=0;
                                end 
                                
                            end
                            for ls=1:lbuf
                                rgblineup(Ax1+ls*m,Ay1+ls*n,1)=1;
                            end
                            end
                    end
            end
            end
        end
    end
    %second
    end
    dep_new(:,:,1)=rgbsymbol;
    dep_new(:,:,2)=rgblineup(:,:,1);
    dep_new(:,:,3)=rgblineup(:,:,2);
end