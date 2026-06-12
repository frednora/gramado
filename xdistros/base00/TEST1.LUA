print ("file: TEST1.LUA:\n");
print ("Hello world!\n");


function fun1()
    print("fun1:")
end

-- comment
function main()
    local x=8
    print("main:")
    fun1()
    -- if x>7 then print("greater than 7")  end
    -- if x<9 then print("Less than 9")     end
end 

print("Calling main()")
main()

