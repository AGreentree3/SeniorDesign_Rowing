%CatchTiming.m
%Program to test identifying the catch, feather, square from the sensor data
%March 21, 2019
%Team FrEE SpEEd
%% Read File, parse data, filter
clear all
clf

%M = csvread('data_long_4-3.txt'); % Read in all raw data
%M = csvread('data_med_4-3.txt'); % Read in all raw data
M = csvread('data_short_4-3.txt'); % Read in all raw data

%Parse Data
millis = M(:,11);
xg = M(:,4);
yg = M(:,5);
zg = M(:,6);
xa = M(:,1);
ya = M(:,2);
za = M(:,3);

%Remove start data since gyro values are zero
xg = xg(100:length(xg)-100);
yg = yg(100:length(yg)-100);
zg = zg(100:length(zg)-100);
xa = xa(100:length(xa)-100);
ya = ya(100:length(ya)-100);
za = za(100:length(za)-100);

% Generate time vector
msPerRead=(millis(1000)-millis(1))/1000;
readsPerSecond=1000/msPerRead;
t = linspace(1,length(xg),length(xg))/readsPerSecond;

%smoothing
%ya = movmean(ya,50);

%% Create figure
figure(1)
hold on
grid on
axis([0, max(t), -1, 2])
plot(t, ya/10, 'k-')
plot(t, zg, 'b-')
plot(t, xg/10, 'g-')

%% Initialize variables
numCatches=0;
numSquares=0;
numFeathers=0;

ready4NewCatch=0;
timeChange=0;
featheringState=0; 
featherTimer=0;
reSquaredForCatch=1;
readsPerStrokeCount = 0;
secsPerStroke = 0;
featherOrSquare=1; %assume starting on the square

%% Input data and analyze
%On plot, red line is catch, magenta line is feather, cyan line is square
for i=1:length(t)
    
    %featheringState = whether feathering is happening
    %featherOrSquare = 0 blade is parallel to water, 1 blade is
    %perpendicular to water
    
    %State machine for featherOrSquare
    if(xg(i)<-4 && ya(i)<10 && featherOrSquare==1)
        featheringState=1;
        featherOrSquare=0;
        featherTimer=0;
        pause(0.2)
        plot([t(i),t(i)],[-8,6],'m-')
        drawnow
        numFeathers=numFeathers+1;
    elseif(xg(i)>1.5 && ya(i)>6 && featherOrSquare==0)
        featheringState=1;
        featherOrSquare=1;
        reSquaredForCatch=1; %for catch state machine
        pause(0.2)
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
        if(timeChange>readsPerSecond*secsPerStroke*0.6)
            ready4NewCatch=1;
            timeChange=0;
        end
        
    elseif(ready4NewCatch==1) %ready for catch
        %criteria depend on if we're feathering or not
        
        %Since **currently** the criteria are pretty much the same for both
        %while feathering and not feathering, we could probably optimize by
        %first checking zg, then checking the rest.
        if(featheringState==1 && zg(i)<-0.4 && zg(i)>zg(i-1)&&reSquaredForCatch==1 )
             pause(0.2)
             plot([t(i),t(i)],[-8,6],'r--')
             drawnow
             ready4NewCatch=0;
             numCatches=numCatches+1;
             secsPerStroke = readsPerStrokeCount/readsPerSecond
             readsPerStrokeCount = 0;
            
        elseif(featheringState==0 && zg(i)<-0.4 && zg(i)>zg(i-1))
             pause(0.2) 
             plot([t(i),t(i)],[-8,6],'r-')
             drawnow
             ready4NewCatch=0;
             numCatches=numCatches+1;
             secsPerStroke = readsPerStrokeCount/readsPerSecond
             readsPerStrokeCount = 0;
             
             reSquaredForCatch=0;
        end   
    end

featheringStateRecord(i)=featheringState; %saves current featheringState
readsPerStrokeCount = readsPerStrokeCount+1;
end

legend('Y accel/10', 'Z gyro', 'X gyro/10');
plot(t,featheringStateRecord,'k')
hold off