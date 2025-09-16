# Overview

This repository is used to store all purple teaming exercises I prepare for my team. All code is written by me (with help of AI) and intended to be run within a simulated environment (do not abuse the contents of this repository for malicious purposes).

# Sessions

### Purple Teaming 01 - Ransomware

Björn Björnson calls, his HR employee Olga Olgason has malware. She suddenly cannot access any sensitive documents anymore, and there is no backup!

During this exercise, I provided my team with a screesnhot, access to Elastic Security and Velociraptor, and a simulated phone call. My team quickly identified the source: a malicious email, containing a link file referring to `%COMSPEC%` (cmd.exe), which executes a .NET dropper, which contains a DLL in its `.rsrc` section, which then gets dropped in the user's profile folder and abuses a DLL search order hijacking attack in an older version of ngentask.exe (stored in WinSxS).

Tools needed: Detect it Easy (DiE), ILSpy/DNSpy, Ghidra

Difficulty: Beginner


<img width="1392" height="867" alt="image" src="https://github.com/user-attachments/assets/c1c89053-1bdf-4f52-b21f-458b0f9d0938" />
