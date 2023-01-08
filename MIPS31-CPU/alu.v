module alu(
    input [31:0] a,
    input [31:0] b,
    input [3:0] aluc,
    output reg [31:0] r,
    output reg zero,
    output reg carry,
    output reg negative,
    output reg overflow
    );
    reg [32:0] r_temp;
    reg [31:0] b0;
    always @(*) begin
    case(aluc)
        4'b0000: begin
            r_temp = a + b;
            r = r_temp[31:0];
            carry = r_temp[32];
            if(r == 0)
                zero = 1;
            else zero = 0;
            //negative
            if(r[31] == 1)
                negative = 1;
            else 
                negative = 0;
        end

    4'b0010: begin
        r=a+b;
        if(~a[31] & ~b[31] & r[31] || a[31] & b[31] & ~r[31])
            overflow = 1;
        else 
            overflow = 0;
        if(r == 0)
            zero = 1;
        else 
            zero = 0;
        if(r[31] == 1)
            negative = 1;
        else 
            negative = 0;
    end

    4'b0001: begin
        r = a - b;
        if(a >= b)
            carry = 0;
        else
            carry = 1;
        if(r == 0)
            zero = 1;
        else 
            zero = 0;
        if(r[31] == 1)
            negative = 1;
        else 
            negative = 0;
    end

    4'b0011: begin
        b0 = ~b + 1;
        r = a + b0;
    if(b[31] != 1'b1)
        if(~a[31] & ~b0[31] & r[31] || a[31] & b0[31] & ~r[31])
            overflow = 1;
        else 
            overflow = 0;
    else 
        if(a[31] == 0)
            overflow = 1;
        else 
            overflow = 0;
        if(r == 0)
            zero = 1;
        else 
            zero = 0;
        if(r[31] == 1)
            negative = 1;
        else 
            negative = 0;
    end

    4'b0100: begin
        r = a & b;
        if(r == 0)
            zero = 1;
        else 
            zero = 0;
        if(r[31] == 1)
            negative = 1;
        else 
            negative = 0;
    end

    4'b0101: begin
        r = a | b;
        if(r == 0)
            zero = 1;
        else 
            zero = 0;
        if(r[31] == 1)
            negative = 1;
        else 
            negative = 0;
    end
       
    4'b0110:  begin
        r = a ^ b;
        if(r == 0)
            zero = 1;
        else 
            zero = 0;
        if(r[31] == 1)
            negative = 1;
        else 
            negative = 0;
    end

    4'b0111:  begin
        r = ~(a | b);
        if(r == 0)
            zero = 1;
        else 
            zero = 0;
        if(r[31] == 1)    
            negative = 1;
        else 
            negative = 0;
    end
    
    4'b1000: begin
        r = {b[15:0], 16'b0};
        if(r == 0)
            zero = 1;
        else 
            zero = 0 ;
        if(r[31] == 1)
            negative = 1;
        else 
            negative = 0;
    end

    4'b1001: begin
        r = {b[15:0], 16'b0};
        if(r == 0)
            zero = 1;
        else 
            zero = 0;
        if(r[31] == 1)
            negative = 1;
        else 
            negative = 0;
    end

    4'b1011: begin
        if(a[31] == 0 && b[31] == 1)
            r = 0;
        else if(a[31] == 1 && b[31] == 0)
            r = 1;
        else if(a[31] == 1 && b[31] == 1)
            r = (a < b) ? 1 : 0;
        else
            r=(a < b) ? 1 : 0;
            if(a == b)
                zero = 1;
            else 
                zero = 0;
            if(r == 1)
                negative=1;
            else 
                negative=0;
    end

    4'b1010: begin
        if(a < b) begin
            r = 1;
            carry = 1;
        end
        else begin
            r = 0;
            carry = 0;
        end
        if(a == b)begin
            r = 0;
            zero = 1;
        end
        else 
            zero = 0;
        if(r[31] == 1)
            negative = 1;
        else 
            negative = 0;
    end
    
    4'b1100: begin
        r = ($signed(b)) >>> a;
        if(a != 0)
            carry = b[a-1];
        else 
            carry = 0;
        if(r == 0)
            zero = 1;
        else 
            zero = 0;
        if(r[31] == 1)
            negative = 1;
        else 
            negative = 0;
    end

    4'b1111: begin
        r = b << a;
        if(a != 0)
            carry = b[32-a];
        else 
            carry = 0;
        if(r == 0)
            zero = 1;
        else 
            zero = 0;
        if(r[31] == 1)
            negative = 1;
        else 
            negative = 0;
    end

    4'b1110: begin
        r = b << a;
        if(a != 0)
            carry = b[32-a];
        else 
            carry = 0;
        if(r == 0)
            zero = 1;
        else 
            zero = 0;
        if(r[31] == 1)
            negative = 1;
        else 
            negative = 0;
    end

    4'b1101: begin
        r = b >> a;
        if(a != 0)
            carry = b[a-1];
        else 
            carry = 0;
        if(r == 0)
            zero = 1;
        else 
            zero = 0;
        if(r[31] == 1)
            negative = 1;
        else 
            negative = 0;
    end
    endcase
end
endmodule
