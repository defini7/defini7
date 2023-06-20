lev :: [Char] -> [Char] -> Int
lev a b
    | length b == 0    = length a
    | length a == 0    = length b
    | head a == head b = lev ta tb
    | otherwise        = 1 + foldr1 min [lev ta b, lev a tb, lev ta tb]
    where
        ta = tail a
        tb = tail b

commands = ["push", "pull", "commit", "update", "fetch", "init"]

main :: IO ()
main = do
    input <- getLine
    let computed = [(c, lev c input) | c <- commands]
    let closest = head (filter ((==) minsnd . snd) computed) where minsnd = minimum (map snd computed)
    
    putStrLn
        (case snd closest of
            0 -> fst closest
            _ -> "Incorrect command, did you mean '" ++ fst closest ++ "'?")
