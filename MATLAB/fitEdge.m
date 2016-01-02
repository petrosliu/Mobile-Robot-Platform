function imgn = fitEdge( img,matchImg,lapImg )
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here
    imgn=img;
lim=20;
   %imgn(633,:)=0;
    for m=1:1:480
        	offset=0;
        	change=0;
            mark=[0,0,0,0];
            match=0;
        for n=2:1:640
            if  (lapImg(n-1,m)~=0) ...
                &&(lapImg(n,m)==0)
                mark=circshift(mark,[0 1]);
                mark(1)=n-1;
                for p=2:1:640
                    if (matchImg(p,m)==0&&matchImg(p-1,m)~=0)
                        match=p;
                        if(match<mark(2)&&match>mark(3)&&(mark(4)~=0))
                            if((match-mark(3))<lim)||((mark(2)-match)<lim)   
                                if (match-mark(3))<(mark(2)-match)
                                    offset=max(mark(4),mark(3)-lim*2);
                                    change=mark(3);
                                    x0=offset:1:change-1;
                                    x1=change:1:match-1;
                                    y0=zeros(1,change-offset);
                                    for k=1:1:change-offset
                                        y0(k)=imgn(k+offset-1,m);
                                    end
                                    p=polyfit(x0,y0,2);
                                    y1=round(polyval(p,x1));
                                    for k=1:1:match-change
                                        imgn(k+change-1,m)=max(min(y1(k),2047),0);
                                    end
                                else
                                    offset=min(mark(1)-3,mark(2)+lim*2);
                                    change=mark(2)-3;
                                    x0=offset:-1:change+1;
                                    x1=change:-1:match+1;
                                    y0=zeros(1,offset-change);
                                    for k=1:1:offset-change
                                        y0(k)=imgn(offset-k+1,m);
                                    end
                                    p=polyfit(x0,y0,2);
                                    y1=round(polyval(p,x1));
                                    for k=1:1:change-match
                                        imgn(change-k+1,m)=max(min(y1(k),2047),0);
                                    end
                                end
                            end
                        end
                    end
                end
            end
        end
    end
    %imgn(633,:)=2047;
end

