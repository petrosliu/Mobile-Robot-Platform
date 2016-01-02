function imgn = removeGhost( img )
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here
    imgn=img;

   imgn(633,:)=0;
 
    for m=1:1:480
        if imgn(1,m)==2047
            mark=0;
        	offset=0;
        	change=0;
        else
            mark=1;
            offset=1;
        	change=0;
        end

        for n=2:1:640
            if (abs(imgn(n,m)-imgn(n-1,m))>50) ...
                &&(imgn(n-1,m)~=2047) ...
                &&(imgn(n,m)~=2047)
                mark=n;
            end
            
            if (imgn(n,m)==2047)&&(imgn(n-1,m)~=2047)
                    change=n;
            end
                if (imgn(n,m)~=2047)&&(imgn(n-1,m)==2047)
                    
                    
                    %%%%%%%%%%%%%%%%%
                    if (offset~=0)&&...
                       ((offset<=change*2-n)||...
                       (abs(imgn(change-1,m)-imgn(n,m))<20))
                        x0=offset:1:change-1;
                        x1=change:1:n-1;
                        y0=zeros(1,change-offset);
                        for k=1:1:change-offset
                            y0(k)=imgn(k+offset-1,m);
                        end
                        p=polyfit(x0,y0,4);
                        y1=round(polyval(p,x1));
                        for k=1:1:n-change
                            imgn(k+change-1,m)=max(min(y1(k),2047),0);
                        end
                        if (abs(imgn(n,m)-imgn(n-1,m))>30)
                            mark=n;
                        end
                        n=mark;
                        change=0;
                        offset=mark;
                    else
                        mark=n;
                        change=0;
                        offset=n;
                    end
                    %%%%%%%%%%%%%%%%%
                end
        end
    end
    imgn(633,:)=2047;
end

