filepath = '/Users/user/Desktop/2020-MUSI6106/samples/'
filename = 'Gstring.wav'
cpp_f = strcat(filename, '_processed.wav');
[x,Fs] = audioread(strcat(filepath, filename));
[x_cpp,Fs] = audioread(strcat(filepath, cpp_f));

y = vibrato_unit(x,Fs,8,0.005);
tf =  norm(x_cpp - y);

audiowrite(strcat(filename,filename, '_matlabprocessed.wav'),y,Fs);

% Vibrato 
function y = vibrato_unit(x,SAMPLERATE,Modfreq,Width)
  ya_alt=0;
  Delay=Width; % basic delay of input sample in sec
  
  DELAY=round(Delay*SAMPLERATE); % basic delay in # samples
  WIDTH=round(Width*SAMPLERATE); % modulation width in # samples
  
  if WIDTH>DELAY 
    error('delay greater than basic delay !!!');
    return;
  end
  
  MODFREQ=Modfreq/SAMPLERATE; % modulation frequency in # samples
  
  LEN=length(x);        % # of samples in WAV-file
  
  L=2+DELAY+WIDTH*2;    % length of the entire delay  
  
  Delayline=zeros(L,1); % memory allocation for delay
  
  y=zeros(size(x));     % memory allocation for output vector
  
  for n=1:(LEN-1)
     
     M=MODFREQ;
     MOD=sin(M*2*pi*n);
     
     ZEIGER=1+DELAY+WIDTH*MOD;
     i=floor(ZEIGER);
     frac=ZEIGER-i;
     
     Delayline=[x(n);Delayline(1:L-1)]; 
     
     %---Linear Interpolation-----------------------------
     y(n,1)=Delayline(i+1)*frac+Delayline(i)*(1-frac);
  end
end


