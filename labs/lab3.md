#Radio Group


define maze 
word maze[9][9]

sender 
send update 


void loop(void)
{
  //
  // Ping out role.  Repeatedly send the current time
  //

  if (role == role_ping_out)
  {
    // First, stop listening so we can talk.
    radio.stopListening();

    // Take the time, and send it.  This will block until complete
    unsigned long time = millis();
    printf("Now sending ...");
    word loc=9; //0001001
    word explored=0;
    word treasure=5;// 101 blue tirangle
    word robot=0; //no robot 
    word walls=12;//1100 north &south
   
    word package=explored<<15|walls<<11|treasure<<8|robot<<7|loc;

    
    // Take the time, and send it.  This will block until complete

    bool ok = radio.write( package,sizeof(word) );

    if (ok)
      printf("ok...");
    else
      printf("failed.\n\r");
    
    // Now, continue listening
    radio.startListening();

    // Wait here until we get a response, or timeout (250ms)
    unsigned long started_waiting_at = millis();
    bool timeout = false;
    while ( ! radio.available() && ! timeout )
      if (millis() - started_waiting_at > 200 )
        timeout = true;

    // Describe the results
    if ( timeout )
    {
      printf("Failed, response timed out.\n\r");
    }
    else
    {
      // Grab the response, compare, and send to debugging spew
      unsigned long got_time;
      radio.read( &got_time, sizeof(unsigned long) );

      // Spew it
      printf("Got response %lu, round-trip delay: %lu\n\r",got_time,millis()-got_time);
    }

    // Try again 1s later
    delay(1000);
  }


receiver's update 



