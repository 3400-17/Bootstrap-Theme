## topics:
  1. Forier Transform
  2. Fast Fourier Transform
## Fourier Transform

Fourier transform is to translate frequency in time domain into frequency domain such that we could analyze the frequencies by directly 
looking at its composition in frequency domain. As it's hard to decompose signals in time domain because several different signals may take plact at the same time so the value of amplitude is actually a combination of several amplitudes belonging to differenet signals.

![decomposition of a signal](https://snag.gy/20la6c.jpg)
This picutre is a sigal in timme domain. It's clear that the singal is made up by six different weighted sinuous signals. 

![singal after Frouier Transform](https://snag.gy/t4s9fy.jpg)
Thrid row represent the signal in time domain and fourth row is the Fourier Transform of the signal. After Fourier Transform, the signal is represented with magnitude of frequency agasint frequency magnitude. The y-axis means how much a certain frequency we have for the signal. 


There are N mutiplies and N-1 addition so the operation is O(N^2) so is there any cheaper way to do fourier transform?

## Fast Frouier Transform
we split X(k) into two groups according to k when k=odd and k=even.

![FFT](https://snag.gy/rc1ACL.jpg)
![FFT](https://snag.gy/V5CnTe.jpg)
![FFT](https://snag.gy/pN9okd.jpg)


<iframe width="560" height="315" src="https://www.youtube.com/embed/EsJGuI7e_ZQ" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>
