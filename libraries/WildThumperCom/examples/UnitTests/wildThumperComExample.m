function wildThumperComExample
% Tests the Wild Thumper Com protocol.  Uses the WHEEL SPEED command:
%  1. START_BYTE
%  2. Length (which is 6 for a wheel speed command)
%  3. Team number (which the sketch has set to 16)
%  4. Command (which is 1 for WHEEL SPEED)
%  5. Left mode (0=Reverse, 1=Brake, 2=Forward)
%  6. Right mode (0=Reverse, 1=Brake, 2=Forward)
%  7. Left duty cycle (0 - 255)
%  8. Right duty cycle (0 - 255)
%  9. CRC
%
% 
clc
s = openSerialConnection();

fprintf('Test #1\n');
dataToSend = [...
    hex2dec('7E'), hex2dec('06'), hex2dec('10'), ...
    hex2dec('01'), hex2dec('02'), hex2dec('02'), ...
    hex2dec('64'), hex2dec('64'), hex2dec('23')];
expectedResult = 'Update wheel speeds to Left Forward 100 Right Forward 100';
performTest(s, dataToSend, expectedResult)

fprintf('Test #2\n');
dataToSend = [126, 6, 16, 1, 0, 0, 100, 100, 39];
expectedResult = 'Update wheel speeds to Left Reverse 100 Right Reverse 100';
performTest(s, dataToSend, expectedResult)

% Invalid messages do no harm.
fwrite(s, hex2dec('7E')); % MATLAB drops first byte oops.
fwrite(s, hex2dec('01')); % MATLAB drops first byte oops.

% Invalid messages do no harm (wrong CRC).
dataToSend = [...
    hex2dec('7E'), hex2dec('06'), hex2dec('10'), ...
    hex2dec('01'), hex2dec('01'), hex2dec('01'), ...
    hex2dec('0'), hex2dec('0'), 236];
fwrite(s, dataToSend);

% Invalid messages do no harm (for a different team).
dataToSend = [...
    hex2dec('7E'), hex2dec('06'), hex2dec('11'), ...
    hex2dec('01'), hex2dec('01'), hex2dec('01'), ...
    hex2dec('0'), hex2dec('0'), 236];
fwrite(s, dataToSend);

% Valid version of tests above.
fprintf('Test #3\n');
dataToSend = [...
    hex2dec('7E'), hex2dec('06'), hex2dec('10'), ...
    hex2dec('01'), hex2dec('01'), hex2dec('01'), ...
    hex2dec('0'), hex2dec('0'), 237];
expectedResult = 'Update wheel speeds to Left Brake 0 Right Brake 0';
performTest(s, dataToSend, expectedResult)

% Invalid messages do no harm (didn't escape for 7E (126) or 7D (125)).
dataToSend = [126, 6, 16, 1, 2, 2, hex2dec('7E'), 125, 240];
fwrite(s, dataToSend);

% Needs the help of the escape byte.
% Sends 7E via 7D then 5E
% Sends 7D (125) via 7D (125) then 5D (93)
fprintf('Test #4\n');
dataToSend = [126, 6, 16, 1, 2, 2, hex2dec('7D'), hex2dec('5E'), 125, 93, 240];
expectedResult = 'Update wheel speeds to Left Forward 126 Right Forward 125';
performTest(s, dataToSend, expectedResult)

fclose(s);
fprintf('Testing complete!\n')
end

function performTest(s, dataToSend, expectedResult)
fwrite(s, dataToSend);
reply = getResponse(s);
fprintf(['Expected : ', expectedResult, '\n']);
fprintf(['Actual   : ', reply, '\n']);
end


function s = openSerialConnection()
% Close any connections first (just in case)
open_ports=instrfind('Type','serial','Status','open');
if ~isempty(open_ports)
    fclose(open_ports);
end

s = serial('COM23', 'Baudrate', 9600, 'Timeout', 0.5, 'Terminator', '');
fopen(s);
fprintf('Serial is open.  Sending data...\n\n');
pause(1); % Serial hardware often needs a moment to initialize.
end


% Function made by Matt DeVries
% Handy but hangs the program if there is no '\n' in the serial buffer.
function response = getResponse(s)
warning off all
response = char(fread(s,1));
while((isempty(response))||(~strcmp(response(end),char(10))))
    response = [response,char(fread(s,1))];
end
warning on all
end
