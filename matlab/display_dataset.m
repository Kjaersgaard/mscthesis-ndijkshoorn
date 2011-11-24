filename = 'C:/Users/Nick/Documents/Thesis/code/cpp/ardrone_slam/ardrone_slam/dataset/052/output.yaml';

[data_alt, data_or, data_accel, data_vel, navdata_euler_angles, gyros_offsets, phys_gyro_temp, phys_gyros, raw_gyros, raw_gyros_110] = loadDataset(filename);

data2_alt = zeros(1,2);
data2_or  = zeros(1,4);
data2_accel=zeros(1,4);
data2_vel = zeros(1,4);

%filename = 'C:/Users/Nick/Documents/Thesis/code/cpp/dataset_collector/dataset_collector/dataset/005/output.yaml';

%[data2_alt, data2_or, data2_accel, data2_vel] = loadDataset(filename);


%%
% ORIENTATIONS

figure('Name','X orientation (milli-deg)');
subplot(3,1,1), plot(data_or(:,1), data_or(:,2));
subplot(3,1,2), plot(data_or(:,1), data_or(:,3));
subplot(3,1,3), plot(data_or(:,1), data_or(:,4));
%suplabel('X orientation (milli-deg)'  ,'t');



figure('Name','navdata_euler_angles');
subplot(3,1,1), plot(navdata_euler_angles(:,1), navdata_euler_angles(:,2));
subplot(3,1,2), plot(navdata_euler_angles(:,1), navdata_euler_angles(:,3));
subplot(3,1,3), plot(navdata_euler_angles(:,1), navdata_euler_angles(:,4));
%suplabel('navdata_euler_angles'  ,'t');

figure('Name','gyros_offsets');
subplot(3,1,1), plot(gyros_offsets(:,1), gyros_offsets(:,2));
subplot(3,1,2), plot(gyros_offsets(:,1), gyros_offsets(:,3));
subplot(3,1,3), plot(gyros_offsets(:,1), gyros_offsets(:,4));
%suplabel('gyros_offsets'  ,'t');

figure('Name','phys_gyro_temp');
subplot(3,1,1), plot(phys_gyro_temp(:,1), phys_gyro_temp(:,2));
subplot(3,1,2), plot(phys_gyro_temp(:,1), phys_gyro_temp(:,3));
subplot(3,1,3), plot(phys_gyro_temp(:,1), phys_gyro_temp(:,4));
%suplabel('phys_gyro_temp'  ,'t');

figure('Name','phys_gyros');

%tmp = raw_gyros_110(:,2) - 1686.9
%tmp = tmp .* 0.5;


tmp = raw_gyros_110(:,2) - 1702.8;
tmp = tmp .* 0.10989011;

subplot(3,1,1), plot(phys_gyros(:,1), phys_gyros(:,2), raw_gyros_110(:,1), tmp, ':');
ylim([-50, 50]);
subplot(3,1,2), plot(phys_gyros(:,1), phys_gyros(:,3));
subplot(3,1,3), plot(phys_gyros(:,1), phys_gyros(:,4));
%suplabel('phys_gyros'  ,'t');



figure('Name','raw_gyros');
%Sensitivity (mV/�/s)
tmp = raw_gyros(:,2) - 1686.9
tmp = tmp .* 0.5;

subplot(3,1,1), plot(raw_gyros(:,1), tmp);
%subplot(3,1,1), plot(raw_gyros(:,1), raw_gyros(:,2));
subplot(3,1,2), plot(raw_gyros(:,1), raw_gyros(:,3));
subplot(3,1,3), plot(raw_gyros(:,1), raw_gyros(:,4));
%suplabel('raw_gyros'  ,'t');

figure('Name','raw_gyros_110');
subplot(3,1,1), plot(raw_gyros_110(:,1), raw_gyros_110(:,2));
subplot(3,1,2), plot(raw_gyros_110(:,1), raw_gyros_110(:,3));
subplot(3,1,3), plot(raw_gyros_110(:,1), raw_gyros_110(:,4));
%suplabel('raw_gyros_110'  ,'t');


%%


% ALT
figure();
plot(data_alt(:,1), data_alt(:,2));

hold on

%plot(data_accel(:,1), data_accel(:,4) + 3650, 'Color', 'r');

%figure();
%plot(navdata_euler_angles(:,1), navdata_euler_angles(:,2));
%ylim([-2500 2500]);
%hold on
%plot(data2_alt(:,1), data2_alt(:,2), 'Color', 'red');
title('Altitude (mm)');



%%


% OR
yrange = [-300, 300];
figure();
subplot(3,1,1), plot(data_or(:,1), data_or(:,2)), hold on, plot(data2_or(:,1), data2_or(:,2), 'Color', 'red');
%ylim(yrange);
title('X orientation (milli-deg)');


% y
subplot(3,1,2), plot(data_or(:,1), data_or(:,3)), hold on, plot(data2_or(:,1), data2_or(:,3), 'Color', 'red');
%ylim(yrange);
title('Y orientation (milli-deg)');

% z
subplot(3,1,3), plot(data_or(:,1), data_or(:,4)), hold on, plot(data2_or(:,1), data2_or(:,4), 'Color', 'red');
%ylim([-180000 180000]);
title('Z orientation (milli-deg)');


std(data_or(:,2))
std(data_or(:,3))

%%


% ACCEL
%yrange = [-500, 500];
figure();

%data_accel(1:300,1)
%mean(data_accel(1:300,4))

subplot(3,1,1), plot(data_accel(:,1), data_accel(:,2));
%ylim([2000 2120]);
%xlim([0 20]);
title('X acceleration (mg)');

% y
subplot(3,1,2), plot(data_accel(:,1), data_accel(:,3));
%ylim([-2000 2120]);
%xlim([0 60]);
title('Y acceleration (mg)');

% z
subplot(3,1,3), plot(data_accel(:,1), data_accel(:,4));
%ylim(yrange);
title('Z acceleration (mg)');

%%

% VEL
yrange = [1650, 1800];
figure();

%subplot(3,1,1), plot(data_vel(:,1), data_vel(:,2), 'Color', 'red');
%ylim(yrange);
%title('X velocity (mm/s)');

% y
%subplot(3,1,2), plot(data_vel(:,1), data_vel(:,3), 'Color', 'red');
%ylim(yrange);
%title('Y velocity (mm/s)');

% z
plot(data_alt(:,1), data_alt(:,2), 'Color', 'r');
hold on;
plot(data_vel(:,1), data_accel(:,4) + 3000);
%plot(data_vel(:,1), 650, 'Color', 'y');
%plot(data_vel(:,1), 550, 'Color', 'y');
%plot(data_vel(:,1), 600, 'Color', 'y');
%ylim(yrange);
title('Z velocity (mm/s)');

%figure();
%A = diff(data_vel(:,1));

%B = cumsum(data_vel(1:4918,4) .* A);
%plot(data_vel(1:4918,1), B);
