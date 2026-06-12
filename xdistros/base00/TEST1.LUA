
print ("\n");
print ("file: TEST1.LUA:\n");
print ("Hello world!\n");


-- Worker
function __fun1()
    print("fun1:")
end

-- comment
function main()

    x=@()
    x[0] = "Frederico"
    print(x[0])  -- ok

    local s = "This is a string"
    print (s) -- ok

    -- local Value=3
    -- print (Value)

    __fun1()
    -- if x>7 then print("greater than 7")  end
    -- if x<9 then print("Less than 9")     end
end 

print("Calling main()")
main()

