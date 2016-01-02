function  pointCloud( rgb , dep )
    map=colorcube(256);
    C=rgb2ind(rgb,map);
    X=uint8(zeros(640,480,256));
    for n=1:1:640
        for m=1:1:480
            if dep(n,m)~= 2047
                X(n,m,dep(n,m)+1)=C(n,m);
            end
        end
    end
%X=smooth3(X);


   
    surface(dep,rgb2ind(rgb,map),...
   'FaceColor','texturemap',...
   'EdgeColor','none',...
   'CDataMapping','direct')
    colormap(map);
    axis([1 640 1 480 1 256 0 255]);
    view(-35,45);
    grid on;
   
    
    
end

