module fsm_controller(clk, rst_n,next_zero,start ,LD_SUM,LD_NEXT,SUM_SEL,NEXT_SEL,A_SEL,DONE);
  parameter START=0,COMPUTE_SUM=1,GET_NEXT=2,S_DONE=3;
  input clk,rst_n,start,next_zero;
  output reg LD_SUM,LD_NEXT,SUM_SEL,NEXT_SEL,A_SEL,DONE;
  reg [1:0] STATE,STATENEXT;
  always @(STATE,start,next_zero) begin
    case(STATE)
      START:begin
        LD_SUM<=0;
        LD_NEXT<=0;
        SUM_SEL<=0;
        NEXT_SEL<=0;
        A_SEL<=0;
        DONE<=0;
        if(start==0)
          STATENEXT<=START;
        else
          STATENEXT<=COMPUTE_SUM;
      end

      COMPUTE_SUM:begin
        LD_SUM<=1;
        LD_NEXT<=0;
        SUM_SEL<=1;
        NEXT_SEL<=1;
        A_SEL<=1;
        DONE<=0;
        STATENEXT<=GET_NEXT;
      end

      GET_NEXT:begin
        if(next_zero==0)
          STATENEXT<=COMPUTE_SUM;
        else
          STATENEXT<=S_DONE;
          LD_SUM<=0;
          LD_NEXT<=1;
          SUM_SEL<=1;
          NEXT_SEL<=1;
          A_SEL<=0;
          DONE<=0;

      end

      S_DONE:begin
        LD_SUM<=0;
        LD_NEXT<=0;
        SUM_SEL<=0;
        NEXT_SEL<=0;
        A_SEL<=0;
        DONE<=1;
      if(start==1)
        STATENEXT<=S_DONE;
      else
        STATENEXT<=START;
      end

    endcase
  end

  always @(posedge clk) begin
    if (rst_n ==0 ) 
      STATE <= START;   
    else
      STATE <= STATENEXT; 
  end
endmodule
