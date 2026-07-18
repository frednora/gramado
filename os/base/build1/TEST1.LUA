
print ("\n");
print ("file: TEST1.LUA:\n");
print ("Hello world!\n");

gString = "This is a global string"
-- gNumber = 42
-- gFloat = 3.14

-- Worker
function __fun1()
    print("fun1:")
end

function main()

    x=@()
    x[0] = "Frederico"
    print(x[0])  -- ok

    local lString = "This is a local string"
    print (lString)

    print (gString)

    -- print (gNumber)
    -- print (gFloat)

    __fun1()
    -- if x>7 then print("greater than 7")  end

end 

print("Calling main()")
main()

