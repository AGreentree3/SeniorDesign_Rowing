%CatchTiming.m
%Program to test identifying the catch, feather, square from the sensor data
%For on the water data
%March 21, 2019
%Team FrEE SpEEd
%% Read File, parse data, filter
clear all
clf

M1 = [];
M2 = [];
%M = csvread('data_long_4-3.txt'); % Read in all raw data
%M = csvread('data_med_4-3.txt'); % Read in all raw data
%M = csvread('data_short_4-3.txt'); % Read in all raw data

M = csvread('ontimeearlyontimelateontime.txt');
%M = csvread('square.txt');
%M = csvread('feather.txt');
%M = csvread('settingtheboat.txt');
%M = csvread('slap.txt');
%M = csvread('buildonfeather.txt');
%M = csvread('squarefeathersquare.txt');
M = M(100:length(M),:); %remove start of data since all zeros

%making sure M1 and M2 are same length
if mod(length(M),2)==1
    M = M(1:length(M)-1,:);
end

for p=1:length(M)
    if (M(p,8)==1)
        M1 = [M1;M(p,:)];
    elseif (M(p,8)==2)
        M2 = [M2;M(p,:)];
    end
end


%%

%Parse Data
millis = M1(:,7);
xg = M1(:,4);
yg = M1(:,5);
zg = M1(:,6);
xa = M1(:,1);
ya = M1(:,2);
za = M1(:,3);

millis2 = M2(:,7);
xg2 = M2(:,4);
yg2 = M2(:,5);
zg2 = M2(:,6);
xa2 = M2(:,1);
ya2 = M2(:,2);
za2 = M2(:,3);

% millis = M(:,11);
% xg = M(:,4);
% yg = M(:,5);
% zg = M(:,6);
% xa = M(:,1);
% ya = M(:,2);
% za = M(:,3);

% Generate time vector
msPerRead=(millis(1000)-millis(1))/1000;
readsPerSecond=1000/msPerRead;
t = linspace(1,length(xg),length(xg))/readsPerSecond;

%smoothing
% ya = movmean(ya,40);

%had to make this change
zg = -zg;

%if oar is port, make the following changes, i think
zg2 = -zg2;
xg2=-xg2;
%% Create figure
figure(1)
hold on
grid on
 axis([0, max(t), -2-5, 2])
%axis([0, max(t), -2, 2])
plot(t, ya/10, 'k-')
plot(t, zg, 'b-')
plot(t, xg/10, 'g-')

%Need to figure out why this works, which is port vs starboard
plot(t, ya2/10 -5, 'k-')
plot(t, zg2 -5, 'b-')
plot(t, xg2/10 -5, 'g-')

%% Initialize variables
numCatches=0;
numSquares=0;
numFeathers=0;

ready4NewCatch=0;
timeChange=0;
featheringState=0; 
featherTimer=0;
%reSquaredForCatch=1;
readsPerStrokeCount = 0;
secsPerStroke = 3;
featherOrSquare=1; %assume starting on the square
sharpDipZg=0;
firstCatch = 0;

%% Input data and analyze
for i=1:length(t)
    
    %featheringState = whether feathering is happening
    %featherOrSquare = 0 blade is parallel to water, 1 blade is
    %perpendicular to water
    
    %issue is that the square up is being seen immediately after the
    %feather, theres a peak in both xg and ya there.
    
    %State machine for featherOrSquare
    if(xg(i)<-9 && ya(i)<10 && featherOrSquare==1)
        featheringState=1;
        featherOrSquare=0;
        featherTimer=0;
        pause(0.1)
        plot([t(i),t(i)],[-8,6],'m-')
        drawnow
        numFeathers=numFeathers+1;
    elseif(xg(i)>2 && ya(i)>6 && zg(i)>0.8 && featherOrSquare==0)
        featheringState=1;
        featherOrSquare=1;
        %reSquaredForCatch=1; %for catch state machine
        pause(0.1)
        plot([t(i),t(i)],[-8,6],'c-')
        drawnow
        numSquares=numSquares+1;
    end
    
    
    if(featheringState==1)
        featherTimer=featherTimer+1; %counts reads since last feathering motion 
        if(featherTimer>(readsPerSecond*secsPerStroke*1.4))
            featheringState=0; %no longer feathering
            featherOrSquare=1;
            featherTimer=0;
        end
    end
    
    
    %State machine for catches    
    if(ready4NewCatch==0) %not ready for catch
        timeChange=timeChange+1;
        if(timeChange>readsPerSecond*secsPerStroke*0.5)
            ready4NewCatch=1;
            timeChange=0;
        end
        sharpDipZg = 0;
        
    elseif(ready4NewCatch==1) %ready for catch

        %NEW! Identifies if a sharp dip has occurred in zg which is indicative
        %of the global minimum of a stroke
        if(zg(i)<-0.4 && ((zg(i)-zg(i-3))<-0.1) )
            sharpDipZg=1;
        end

        
        if(featheringState==1 && zg(i-3)<-0.45 && zg(i)>zg(i-1) && zg(i-1)>=zg(i-2) && zg(i-2)>=zg(i-3)...
                &&sharpDipZg==1) % && reSquaredForCatch==1)
             pause(0.1)
             plot([t(i),t(i)],[-8,6],'r-')
             drawnow
             ready4NewCatch=0;
             numCatches=numCatches+1;
             
             if firstCatch~=0
                 secsPerStroke = readsPerStrokeCount/readsPerSecond
             end
             readsPerStrokeCount = 0;
             firstCatch=1;


        elseif(featheringState==0 && zg(i-3)<-0.45 && zg(i)>zg(i-1) && zg(i-1)>=zg(i-2) && zg(i-2)>=zg(i-3) ...
                && (abs(ya(i-2)-ya(i-3)+abs(ya(i-1)-ya(i-2))) > 1.0 )...
                && sharpDipZg==1)    
             pause(0.1) 
             plot([t(i),t(i)],[-8,6],'r--')
             drawnow
             ready4NewCatch=0; 
             numCatches=numCatches+1;
             
             if firstCatch~=0
                 secsPerStroke = readsPerStrokeCount/readsPerSecond
             end
             readsPerStrokeCount = 0;
             firstCatch=1;
             
             %reSquaredForCatch=0;
        end   
    end

featheringStateRecord(i)=featheringState; %saves current featheringState
readsPerStrokeCount = readsPerStrokeCount+1;

end



% zg=zg2;
% ya=ya2;
% xg=xg2;
% for i=1:length(t)
%     
%     %featheringState = whether feathering is happening
%     %featherOrSquare = 0 blade is parallel to water, 1 blade is
%     %perpendicular to water
%     
%     %issue is that the square up is being seen immediately after the
%     %feather, theres a peak in both xg and ya there.
%     
%     %State machine for featherOrSquare
%     if(xg(i)<-9 && ya(i)<10 && featherOrSquare==1)
%         featheringState=1;
%         featherOrSquare=0;
%         featherTimer=0;
%         pause(0.1)
%         %plot([t(i),t(i)],[-8,6],'m-')
%         drawnow
%         numFeathers=numFeathers+1;
%     elseif(xg(i)>2 && ya(i)>6 && zg(i)>0.8 && featherOrSquare==0)
%         featheringState=1;
%         featherOrSquare=1;
%         reSquaredForCatch=1; %for catch state machine
%         pause(0.1)
%         %plot([t(i),t(i)],[-8,6],'c-')
%         drawnow
%         numSquares=numSquares+1;
%     end
%     
%     
%     if(featheringState==1)
%         featherTimer=featherTimer+1; %counts reads since last feathering motion 
%         if(featherTimer>(readsPerSecond*secsPerStroke*1.4))
%             featheringState=0; %no longer feathering
%             featherOrSquare=1;
%             featherTimer=0;
%         end
%     end
%     
%     
%     %State machine for catches    
%     if(ready4NewCatch==0) %not ready for catch
%         timeChange=timeChange+1;
%         if(timeChange>readsPerSecond*secsPerStroke*0.5)
%             ready4NewCatch=1;
%             timeChange=0;
%         end
%         sharpDip = 0;
%         
%     elseif(ready4NewCatch==1) %ready for catch
%         %criteria depend on if we're feathering or not
%         if(zg(i)<-0.4 && ((zg(i)-zg(i-3))<-0.1) )
%             sharpDip=1;
%         end
%         %Since **currently** the criteria are pretty much the same for both
%         %while feathering and not feathering, we could probably optimize by
%         %first checking zg, then checking the rest.
%         if(featheringState==1 && zg(i-3)<-0.45 && zg(i)>zg(i-1) && zg(i)>zg(i-2) && zg(i-1)>=zg(i-2) && zg(i-2)>=zg(i-3)...
%                 &&sharpDip==1) % && reSquaredForCatch==1)
%              pause(0.1)
%              plot([t(i),t(i)],[-8,6],'r-')
%              drawnow
%              ready4NewCatch=0;
%              numCatches=numCatches+1;
%              
%              if firstCatch~=0
%                  secsPerStroke = readsPerStrokeCount/readsPerSecond
%              end
%              readsPerStrokeCount = 0;
%              firstCatch=1;
% 
% 
%         elseif(featheringState==0 && zg(i-3)<-0.45 && zg(i)>zg(i-1) && zg(i)>zg(i-2) && zg(i-1)>=zg(i-2) && zg(i-2)>=zg(i-3) ...
%                 && (abs(ya(i-2)-ya(i-3)+abs(ya(i-1)-ya(i-2))) > 1.0 )...
%                 && sharpDip==1)    
%         %elseif(featheringState==0 && zg(i)<-0.4 && zg(i)>zg(i-1) && zg(i)>zg(i-2))
%              pause(0.1) 
%              plot([t(i),t(i)],[-8,6],'r--')
%              drawnow
%              ready4NewCatch=0; 
%              numCatches=numCatches+1;
%              
%              if firstCatch~=0
%                  secsPerStroke = readsPerStrokeCount/readsPerSecond
%              end
%              readsPerStrokeCount = 0;
%              firstCatch=1;
%              
%              reSquaredForCatch=0;
%         end   
%     end
% 
% featheringStateRecord(i)=featheringState; %saves current featheringState
% readsPerStrokeCount = readsPerStrokeCount+1;
% 
% end

legend('Y accel/10', 'Z gyro', 'X gyro/10');
plot(t,featheringStateRecord,'k')
hold off