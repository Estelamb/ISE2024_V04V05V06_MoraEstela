t <html><head><title>Time and Date</title></head>
i pg_header.inc
t <h2 align=center><br>RTC Module Control</h2>
t <p><font size="2">This page allows you to see and change the <b>Time and Date</b>.<br><br>
t  This Form uses a <b>POST</b> method to send data to a Web server.</font></p>
t <form action=rtc.cgi method=post name=cgi>
t <input type=hidden value="rtc" name=pg>
t <table border=0 width=99%><font size="3">
t <tr bgcolor=#aaccff>
t  <th width=40%>Item</th>
t  <th width=60%>Setting</th></tr>
# Here begin data setting which is formatted in HTTP_CGI.C module
t <tr><td><img src=pabb.gif>Time: </td>
c h 1 <td><input type=text name=time size=20 maxlength=20 value="%s"></td></tr>
t <tr><td><img src=pabb.gif>Date: </TD>
c h 2 <td><input type=text name=date size=20 maxlength=20 value="%s"></td></tr>
t </font></table>
# Here begin button definitions
t <p align=center>
t <input type=submit name=set value="Send" id="sbm">
t </p></form>
i pg_footer.inc
. End of script must be closed with period.
