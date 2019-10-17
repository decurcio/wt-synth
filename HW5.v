module arbiter(clock, reset, R0, R1, G0, G1);
    //ECE464 HW5
    //Design an arbiter to handle two request lines
    //and two grant lines
    input clock;
    input reset;
    input R0, R1; //Request lines
    output G0, G1;//Grant lines

    paremeter [2:0] // synopsys enum states
        S0 = 3'b000,
        S1 = 3'b001,
        S2 = 3'b010,
        S3 = 3'b011,
        S4 = 3'b100;

    reg [2:0] /* synopsys enum states */ current_state, next_state;
        //synopsys state_vector current_state

    reg G0, G1; //Output registers

    /*---------Sequential Logic ------------*/
    always@(posedge clock)
    begin
        if(!reset) current_state <= S0; //Synchronous Reset
        else current_state <= next_state; //Change the state
    end

    /*------Next state logic/ output logic-------*/
    always@(current_state)
    begin
        G0 <= 0; G1 <= 0; //Default values, prevents latches
        case (current_state) // synopsys full_case parallel_case
            S0: begin
                //State 0, arbiter idle
                G0 <= 1'b0;
                G1 <= 1'b0;
                if(R0 && R1) next_state <= S3;
                else if(R0 && !R1) next_state <= S1;
                else if(!R0 && R1) next_state <= S2;
                else next_state <= S0;
            end
            S1: begin
                //State 1, R0 but not R1
                G0 <= 1'b1;
                G1 <= 1'b0;
                if(R0 && R1) next_state <= S3;
                else if(R0 && !R1) next_state <= S1;
                else if(!R0 && R1) next_state <= S2;
                else next_state <= S0;
            end
            S2: begin
                //State 2, R1 but not R0
                G0 <= 1'b0;
                G1 <= 1'b1;
                if(R0 && R1) next_state <= S3;
                else if(R0 && !R1) next_state <= S1;
                else if(!R0 && R1) next_state <= S2;
                else next_state <= S0;
            end
            S3: begin
                //State 3, both R0 and R1 at the same time
                G0 <= 1'b1;
                G1 <= 1'b0;
                next_state <= S4;
            end
            S4: begin
                //State 4, both R0 and R1 at the same time
                //Granted R0 already, now grant R1
                G0 <= 1'b0;
                G1 <= 1'b1;
                if(R0 && R1) next_state <= S3;
                else if(R0 && !R1) next_state <= S1;
                else if(!R0 && R1) next_state <= S2;
                else next_state <= S0;
            end
            default: next_state <= S0; //Default, resets if noise causes illegal state
        endcase
    end

endmodule