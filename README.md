inlineHookLL tries to hook LoadLibraryA in kernel32.dll by using IAT hooking.(and yes,the comment part include a MessageBoxA hook as well). inlineHookLL doesn't do anything in hooked function so you may not notice whether the hooking is successful or not.

HookFunction will try to replace an old function address at a position on IAT,which is returned by FoundIAT function,with a new custom function address. FoundIAT is very like what we did in PEReader.

inlineHookLL 就是一个IAT hook的小例子，实际使用中效果还可以，但是hook技术本身在反外挂方面已经不太好用了。
